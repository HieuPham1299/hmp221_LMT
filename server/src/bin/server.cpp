#include <vector>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "hmp221.hpp"
#include <fstream>
#include <sys/stat.h>
#include "hashmap.h"
#define KEY 42

using namespace std;
string checkMessageType(vec bytes);
void processRequest(unsigned int newsockfd, vec responseBytes, HashMap *map);
void processReceive(vec responseBytes, HashMap *map);
void pushToBuffer(char buffer[], vec *serializedMessageStruct);
void processClientConnection(int newsockfd, char *hostName, int hostPortNo, int comPortNo);
vec serializeFile(HashMap *map, string fileName);

int main(int argv, char **argc)
{
    bool hasHostNameFlag = false;
    // char *mode;
    // char *filePath;
    char *serverInfo;
    char *hostName;
    int hostPortNo;
    int comPortNo = 8888;   // Port number that a socket used to communicate with children processes bound to. 
                            // Data delivered through this socket comes from the hashmap
    unsigned int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n, pid;
    for (int i = 1; i < argv; i++)
    {
        char *currentString = *(argc + i);
        if (strcmp(currentString, "--hostname") == 0)
        {
            hasHostNameFlag = true;
            serverInfo = *(argc + i + 1);
        }
    }

    if (!hasHostNameFlag)
    {
        cout << "No --hostname flag found." << endl;
        return 1;
    }

    // extract hostname and port number
    hostName = strtok(serverInfo, ":");
    hostPortNo = atoi(strtok(NULL, ":"));

    printf("Server is running at %s:%d\n", hostName, hostPortNo);

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(hostPortNo);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    // Establish a socket for communication between parent and children processes
    int comSockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (comSockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(comPortNo);

    /* Now bind the host address using bind() call.*/
    if (bind(comSockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here
     * process will go in sleep mode and will wait
     * for the incoming connection
     */

    if (listen(sockfd, 5) == -1) {
        perror("ERROR on binding");
        exit(1);
    }
    clilen = sizeof(cli_addr);

    HashMap *map = new HashMap(100);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd < 0)
        {
            perror("ERROR on accept new client connection");
            exit(1);
        }

        pid = fork();

        if (pid < 0)
        {
            perror("ERROR on forking new process");
            exit(1);
        }

        if (pid == 0)
        {
            /* This is the child process */
            close(sockfd);
            close(comSockfd);
            processClientConnection(newsockfd, hostName, hostPortNo, comPortNo++);
            exit(0);
        }
        else
        {
            close(newsockfd);
            if (listen(comSockfd, 5) == -1)
            {
                perror("ERROR on binding parent-child connection socket");
                exit(1);
            }

            // Listen to communication request from child process
            int newComSockfd = accept(comSockfd, (struct sockaddr *)&cli_addr, &clilen);

            if (newComSockfd < 0)
            {
                perror("ERROR on accept from child process");
                exit(1);
            }
            char buffer[65536];
            bzero(buffer, 65536);
            n = read(newComSockfd, buffer, 65536);
            
            // Decrypted the message and push all bytes to a vector
            vec responseBytes;
            for (int i = 0; i < 65536; i++)
            {
                responseBytes.push_back(buffer[i] ^ KEY);
            }

            string messageType = checkMessageType(responseBytes);

            if (messageType.compare("subscribe") == 0)
            {
                processRequest(newComSockfd, responseBytes, map);
                printf("Terminating connection with %s:%d.\n", hostName, hostPortNo);
                printf("--------------------------------\n");
            }
            else
            {
                processReceive(responseBytes, map);
                printf("Terminating connection with %s:%d.\n", hostName, hostPortNo);
                printf("--------------------------------\n");
            }
            close(newComSockfd);
        }
    } /* end of while */

    return 0;
}

void ppause(int n) {
  int x = 0;
  while(x < n) {
    x+=1;
  }
}

/**
 * @brief process client request to a child process
 * 
 * @param hostName Name of parent process
 * @param hostPortNo Port number that the parent's socket is bound to
 * @param comPortNo Port number of parent's communication socket
 */
void processClientConnection(int newsockfd, char *hostName, int hostPortNo, int comPortNo) {

    // Creating a child-parent connection:
    char buffer[65536];
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /* Create a socket point to communicate with comSockfd in parent process*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(hostName);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(comPortNo);

    ppause(0x0FFFFFFF);
    /* Now connect to the server to establish parent-child communication channel*/
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR establishing communication channel with parent process");
        exit(1);
    }

    // Take client request message
    bzero(buffer, 65536);
    n = read(newsockfd, buffer, 65536);

    if (n < 0)
    {
        perror("ERROR reading from client socket");
        exit(1);
    }

    n = write(sockfd, buffer, 65536);
    if (n < 0)
    {
        perror("ERROR writing to parent-child socket");
        exit(1);
    }

    bzero(buffer, 65536);
    n = read(sockfd, buffer, 65536);
    if (n < 0)
    {
        perror("ERROR reading from parent-child socket");
        exit(1);
    }

    n = write(newsockfd, buffer, 65536);
    if (n < 0)
    {
        perror("ERROR writing to client socket");
        exit(1);
    }
    close(sockfd);
}

string checkMessageType(vec bytes) {
    if (bytes.size() < 5) {     // FIXME: Check for the exact byte
        throw;
    }
    vec message_slice = hmp221::slice(bytes, 2, 10);
    string message_string = hmp221::deserialize_string(message_slice);
    std::cout << message_string << std::endl;
    if (message_string.compare("Message") == 0)
    {
        return "publish";
    }
    return "subscribe";
}

void processRequest(unsigned int newComSockfd, vec responseBytes, HashMap *map) {
    struct Request requestStruct = hmp221::deserialize_request(responseBytes);
    string channel = requestStruct.name;
    vec testBytes = hmp221::serialize((string)"Hello");
    map->put((string) "test", testBytes);
    if (map->get(channel) == (vector<unsigned char>) NULL) {
        perror("No message published on this channel.");
        return;
    }
    
    struct Message messageStruct = {channelName : channel, contentBytes : map->get(channel)};
    vec serializedMessageStruct = hmp221::serialize(messageStruct);
    // Encrypt the bytes
    for (int i = 0; i < serializedMessageStruct.size(); i++)
    {
        serializedMessageStruct[i] ^= KEY;
    }
    char buffer[serializedMessageStruct.size()];

    // Push the encrypted bytes to buffer
    pushToBuffer(buffer, &serializedMessageStruct);
    printf("Sending %ld bytes\n", serializedMessageStruct.size());

    /* Send message to the server */
    int n = write(newComSockfd, buffer, serializedMessageStruct.size());
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
    cout << "Message sent.\nDone." << endl;
}

void processReceive(vec responseBytes, HashMap *map)
{
    struct Message messageStruct = hmp221::deserialize_message(responseBytes);
    printf("Received a message of %ld bytes\n", messageStruct.contentBytes.size());
    string channel = messageStruct.channelName;
    map->put(channel, messageStruct.contentBytes);
    std::cout << channel  << std::endl;
}

void pushToBuffer(char buffer[], vec *bytesP)
{
    int i = 0;
    for (; i < bytesP->size(); i++)
    {
        buffer[i] = (*bytesP)[i];
    }
}