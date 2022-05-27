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

void printFlagError();
int connectToServer(int portno, char *hostName);
void sendToServer(int portNo, char *hostName, char *filePath);
void requestFromServer(int portNo, char *hostName, char *filePath);
vec serializeFile(char *filePath);
void pushToBuffer(char *bufferP, vec *bytesP);


int main(int argv, char **argc)
{
    bool hasValidModeFlag = false;
    char *mode;
    char *filePath;
    char *serverInfo;
    char *hostName;
    int portNo;
    for (int i = 1; i < argv; i++)
    {
        char *currentString = *(argc + i);
        if (currentString[0] == '-' && currentString[1] == '-')
        {
            if (strcmp(currentString, "--request") == 0)
            {
                mode = "request";
                hasValidModeFlag = true;
                filePath = *(argc + i + 1);
                break;
            }
            else if (strcmp(currentString, "--send") == 0)
            {
                mode = "send";
                hasValidModeFlag = true;
                filePath = *(argc + i + 1);
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
    if (strcmp(mode, "send") == 0) {
        sendToServer(portNo, hostName, filePath);
    } else {
        requestFromServer(portNo, hostName, filePath);
    }
}

/* Printing debugging message when there is an invalid flag */
void printFlagError()
{
    cout << "ERROR: Expected mode" << endl;
    cout << "usage: client --send filename" << endl;
    cout << "usage: client --request filename" << endl;
}

/* Request a file from the server */
void requestFromServer(int portno, char *hostName, char *fileName) {
    // Connect to server and get the socket descriptor
    int sockfd = connectToServer(portno, hostName);
    printf("Requesting file \"%s\"\n", fileName);
    struct Request requestStruct = {name : fileName};
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

    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }

    vec responseBytes;
    for (int i = 0; i < 65536; i++) {
        responseBytes.push_back(buffer[i]^KEY);
    }

    struct Message messageStruct = hmp221::deserialize_message(responseBytes);

    printf("Received a file containing %ld bytes\n", messageStruct.contentBytes.size());
    mkdir("received", 0);
    string path = "received/" + (string)fileName;

    std::cout << hmp221::deserialize_string(messageStruct.contentBytes) << std::endl;

    std::cout << "Saved file in: \"" + path + "\"" << std::endl;
    printf("Terminating connection with %s:%d.\n", hostName, portno);
} 

/* Send a file to the server */
void sendToServer(int portno, char *hostName, char *filePath)
{
    // Connect to server and get the socket descriptor
    // int sockfd = connectToServer(portno, hostName);

    // // Construct a filePathString from a char pointer
    // string filePathString(filePath);

    // // Extract the file name. Reference: https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
    // string fileName = filePathString.substr(filePathString.find_last_of("/\\") + 1);

    // printf("Sending file \"%s\"\n", filePath);
    // struct Message messageStruct = {channelName : fileName, bytes : serializeFile(filePath)};
    // printf("Read file: %ld bytes\n", messageStruct.bytes.size());
    // vec serializedmessageStruct = hmp221::serialize(messageStruct);

    // // Encrypt the bytes
    // for (int i = 0; i < serializedmessageStruct.size(); i++)
    // {
    //     serializedmessageStruct[i] ^= KEY;
    // }
    // char buffer[serializedmessageStruct.size()];

    // // Push the encrypted bytes to buffer
    // pushToBuffer(buffer, &serializedmessageStruct);
    // printf("Sending %ld bytes\n", serializedmessageStruct.size());

    // /* Send message to the server */
    // int n = write(sockfd, buffer, serializedmessageStruct.size());
    // if (n < 0)
    // {
    //     perror("ERROR writing to socket");
    //     exit(1);
    // }

    // std::cout << "Message sent.\nDone." << std::endl;
}

/* Connect to the server using the given hostname and port number */
int connectToServer(int portno, char *hostName) {
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

vec serializeFile(char *filePath)
{
    FILE *file;
    vec bytes;
    int c;

    // Open as binary file
    file = fopen(filePath, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(0);
    }
    else
    {
        while (1)
        {
            c = fgetc(file);
            if (c == EOF)
            {
                break;
            }
            bytes.push_back(c);
        }
        fclose(file);
    }
    return bytes;
}

void pushToBuffer(char buffer[], vec *bytesP)
{
    int i = 0;
    for (; i < bytesP->size(); i++)
    {
        buffer[i] = (*bytesP)[i];
    }
}