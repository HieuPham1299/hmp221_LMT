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
#define KEY 42

using namespace std;

// Declared methods used to avoid compiler error
void printFlagError();
int connectToServer(int portno, char *hostName);
void publish(int portNo, char *hostName, char *channel, char *message);
void subscribe(int portNo, char *hostName, char *channel);
void pushToBuffer(char *bufferP, vec *bytesP);

int main(int argv, char **argc)
{
    bool hasValidModeFlag = false;
    char *mode;
    char *channel;
    char *message;
    char *serverInfo;
    char *hostName;
    int portNo;
    for (int i = 1; i < argv; i++)
    {
        char *currentString = *(argc + i);
        if (currentString[0] == '-' && currentString[1] == '-')
        {
            if (strcmp(currentString, "--subscribe") == 0)
            {
                mode = "subscribe";
                hasValidModeFlag = true;
                channel = *(argc + i + 1);
                break;
            }
            else if (strcmp(currentString, "--publish") == 0)
            {
                mode = "publish";
                hasValidModeFlag = true;
                channel = *(argc + i + 1);
                message = *(argc + i + 2);
                break;
            }
            else if (strcmp(currentString, "--hostname") == 0)
            {
                serverInfo = *(argc + i + 1);
            }
        }
    }

    if (!hasValidModeFlag)
    {
        printFlagError();
        return 1;
    }

    // extract hostname and port number
    hostName = strtok(serverInfo, ":");
    portNo = atoi(strtok(NULL, ":"));
    if (strcmp(mode, "publish") == 0)
    {
        publish(portNo, hostName, channel, message);
    }
    else
    {
        subscribe(portNo, hostName, channel);
    }
}

/* Printing debugging message when there is an invalid flag */
void printFlagError()
{
    cout << "ERROR: Expected mode" << endl;
    cout << "usage: client --subscribe [channel]" << endl;
    cout << "usage: client --publish [channel] [message]" << endl;
}

/**
 * @brief Subscribe to a channel from the server
 * @param portNo server's port number
 * @param hostName server's name
 * @param channel channel's name
 */
void subscribe(int portno, char *hostName, char *channel)
{
    // Connect to server and get the socket descriptor
    int sockfd = connectToServer(portno, hostName);
    printf("Reading from channel \"%s\"\n", channel);
    struct Request requestStruct = {name : channel};
    vec serializedRequest = hmp221::serialize(requestStruct);
    // Encrypt the bytes
    for (int i = 0; i < serializedRequest.size(); i++)
    {
        serializedRequest[i] ^= KEY;
    }

    char buffer[65536] = {};
    bzero(buffer, 65536);

    // Push the encrypted bytes to buffer
    pushToBuffer(buffer, &serializedRequest);
    printf("Sending %ld bytes\n", serializedRequest.size());

    /* Send message to the server */
    int n = write(sockfd, buffer, serializedRequest.size());

    if (n < 0)
    {
        perror("ERROR sending request");
        exit(1);
    }
    cout << "Message Sent." << endl;

    bzero(buffer, 65536);

    n = read(sockfd, buffer, 65536);

    // When n < 0, there was an error reading from the socket
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }

    vec responseBytes;
    for (int i = 0; i < 65536; i++)
    {
        responseBytes.push_back(buffer[i] ^ KEY);
    }

    // This is the case where the topic does not exist

    struct Message messageStruct = hmp221::deserialize_message(responseBytes);
    if (messageStruct.contentBytes == (vector<unsigned char>) NULL) {
        std::cout << "NULL" << std::endl;
    }
    printf("Received a %ld-byte message\n", messageStruct.contentBytes.size());

    std::cout << hmp221::deserialize_string(messageStruct.contentBytes) << std::endl;

    printf("Terminating connection with %s:%d.\n", hostName, portno);
}

/**
 * @brief Send a message to the server
 * @param portNo server's port number
 * @param hostName server's name
 * @param channel channel's name
 * @param message message to be published
 */
void publish(int portno, char *hostName, char *channel, char *message)
{
    // Connect to server and get the socket descriptor
    int sockfd = connectToServer(portno, hostName);

    // Construct a channelString from a char pointer
    string channelString(channel);

    printf("Sending message to channel \"%s\"\n", channel);
    struct Message messageStruct = {channelName : channel, contentBytes : hmp221::serialize(string(message))};
    printf("Read message: %ld bytes\n", messageStruct.contentBytes.size());
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
    int n = write(sockfd, buffer, serializedMessageStruct.size());
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }

    std::cout << "Message sent.\nDone." << std::endl;
}

/**
 * @brief Connect to the server using the given hostname and port number
 * @param portno server's port number
 * @param hostname server's name, usually represented by IP adress
 */
int connectToServer(int portno, char *hostName)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /* Create a socket point */
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
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    printf("Connecting to %s:%d.\n", hostName, portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }
    std::cout << "Successfully connected to server." << endl;
    return sockfd;
}

void pushToBuffer(char buffer[], vec *bytesP)
{
    int i = 0;
    for (; i < bytesP->size(); i++)
    {
        buffer[i] = (*bytesP)[i];
    }
}