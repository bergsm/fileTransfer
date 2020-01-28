/************************************************************************
* ftserver.c
* Author - Shawn Berg
* Description - A Client program that connects with a server program and sends messages
* Date - 3/6/18
* External Sources: Referenced beej's socket programming guide for setting up TCP socket
* URL: https://beej.us/guide/bgnet/html/single/bgnet.html
************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MESSAGE_MAX_SIZE 10000
#define FILENAME_MAX_SIZE 100

/***********************************************
* Function name: error
* Description: This function helps to print errors
* Parameters: const char *msg
* Returns: void
************************************************/
void error(const char *msg) { fflush(stdout); perror(msg); exit(0); }

/***********************************************
* Function name: sendMessage
* Description: This function sends a message to the server.
* Parameters: char* input, int sockfd (socket file descriptor)
* Returns: void
* Preconditions: Message available to send
* Postconditions: Message sent via socket
************************************************/
void sendMessage(char* msg, int sockfd)
{
    // prepare message for sending
    char message[MESSAGE_MAX_SIZE];
    memset(message, '\0', MESSAGE_MAX_SIZE);
    strcpy(message, msg);

    // send message
    send(sockfd, message, strlen(message), 0);
}

/***********************************************
* Function name: sendFileTxt
* Description: This function reads a file and sends it via socket
* Parameters: char* fileName, int sockfd
* Returns: void
* Preconditions: a file name from the client and a connected socket
* Postconditions: file sent to client
************************************************/
void sendFileTxt(char* fileName, int sockfd)
{
    // send file through socket
    FILE *fp;
    char buff[MESSAGE_MAX_SIZE];
    char msg[MESSAGE_MAX_SIZE];
    strcpy(msg, "");
    fp = fopen(fileName, "r");
    while(fgets(buff, sizeof buff, fp) != NULL)
        strcat(msg, buff);
    sendMessage(msg, sockfd);

}


/***********************************************
* Function name: getDirContents
* Description: gets the current directory contents and returns them as a string
* Parameters: None
* Returns: char*
* Preconditions: None
* Postconditions: Directory contents returned as string
************************************************/
void getDirContents(char* contents)
{
    FILE *fp;
    char file[FILENAME_MAX_SIZE];
    strcpy(contents, "");

    fp = popen("ls", "r");
    while(fgets(file, FILENAME_MAX_SIZE, fp) != NULL)
    {
        strcat(contents, file);
    }
    fflush(stdout);
}

/***********************************************
* Function name: getServerSockInfo
* Description: This funciton fills a struct with information about the intended socket
* Parameters: none
* Returns: struct addrinfo sockinfo (to be used in other functions)
* Preconditions: User ready to connect to server
* Postconditions: Server socket info stored in memory
************************************************/
struct addrinfo getServerSockInfo()
{
    struct addrinfo sockinfo;

    memset(&sockinfo, 0, sizeof(sockinfo));
    sockinfo.ai_family = AF_UNSPEC;
    sockinfo.ai_socktype = SOCK_STREAM;
    sockinfo.ai_flags = AI_PASSIVE;

    return sockinfo;
}


/***********************************************
* Function name: recvMessage
* Description: This function listens to the socket and returns the response
* Parameters: int sockfd (socket file descriptor)
* Returns: char*
* Preconditions: Socket connected and message sent
* Postconditions: Message received and returned
************************************************/
void recvMessage(char* msg, int sockfd)
{
    // listen for response
    int bytes;
    memset(msg, '\0', MESSAGE_MAX_SIZE);
    bytes = recv(sockfd, msg, MESSAGE_MAX_SIZE, 0);

    if (bytes == 0)
        exit(1);
}

/***********************************************
* Function name: connectToServer
* Description: This function takes information about the type of socket and the socket address
* and creates a connection to the server
* Parameters: char* hostname, char* port, struct addrinfo sockinfo (filled in)
* Returns: int sockfd (file descriptor for the socket)
* Preconditions: Socket address info stored in memory
* Postconditions: Connected to server via socket
************************************************/
int connectToServer(char* hostname, char* port)
{
    int sockfd;
    int status;
    struct addrinfo* servinfo;
    struct addrinfo sockinfo = getServerSockInfo();

    // get server info from socket address
    if ((status = getaddrinfo(hostname, port, &sockinfo, &servinfo)) != 0)
        error("Error getting address info");

    // create socket file descriptor
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd < 0) error("CLIENT: ERROR opening socket");

    // connect to socket
    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
        error("CLIENT: ERROR connecting");
    else
    {
        // send initial message with portnum to server
        //send(sockfd, port, strlen(port), 0);
        //printf("Connected to server, say hello!\n");
    }

    return sockfd;
}


/***********************************************
* Function name: acceptClient
* Description: This function takes a port and listens for a connection from a client
* Parameters: int* newSock, char* port
* Returns: void
* Preconditions: port number from command line
* Postconditions: Accepted connection from client
************************************************/
//int acceptClient(char* port)
void acceptClient(int* newSock, char* port)
{
    //int newSock;
    int sockfd;
    int status;
    struct addrinfo* servinfo;
    struct sockaddr_storage incomingAddr;
    struct addrinfo sockinfo = getServerSockInfo();
    //char* hostname = "127.0.0.1";

    // get server info from socket address
    if ((status = getaddrinfo(NULL, port, &sockinfo, &servinfo)) != 0)
        error("Error getting address info");

    // create socket file descriptor
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd < 0) error("SERVER: ERROR opening socket");

    // listen for connections
    bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    listen(sockfd, 10);

    socklen_t incomingAddrSize = sizeof incomingAddr;
    *newSock = accept(sockfd, (struct sockaddr *)&incomingAddr, &incomingAddrSize);

    close(sockfd);

    //return newSock;
}

/***********************************************
* Function name: getSockAddr
* Description: This function gets the hostname and port from the command line arguments and creates
* a socket address for them.
* Parameters: int argc (from main), char* argv[](from main), char* sockAddr[2]
* Returns: void
* Preconditions: User started program properly with command line arguments
* Postconditions: Server socket address stored in memory
************************************************/
void getSockAddr(int argc, char* argv[], char* sockAddr[2])
{
    // get hostname and port from command line
    if (argc < 2)
    {
        printf("Run ftserver with command \"./ftserver <port>\"\n");
        exit(0);
    }
    else
    {
        sockAddr[0] = "127.0.0.1";
        sockAddr[1] = argv[1];
    }
}


/***********************************************
* Function name: fileExists
* Description: This function checks to see if a file exists in the current
* directory
* Parameters: char* fileName
* Returns: 1 for true, 0 for false
* Preconditions: fileName for file in question
* Postconditions: integer value representing the existence of the file
************************************************/
int fileExists(char* fileName)
{
    char contents[MESSAGE_MAX_SIZE];
    getDirContents(contents);
    // if file exists
    if (strstr(contents, fileName))
        return 1;
    else
        return 0;
}

/***********************************************
* Function name: parseCommand
* Description: This function takes a string and splits it into an
* array of strings
* Parameters: char* command, char* commands[]
* Returns: void
* Preconditions: command received from client
* Postconditions: command parsed into an array
************************************************/
void parseCommand(char command[MESSAGE_MAX_SIZE], char* commands[6])
{
    char* tok;
    tok = strtok(command," ");
    commands[0] = tok;
    int idx = 1;
    while(tok != NULL)
    {
        tok = strtok(NULL, " ");
        commands[idx] = tok;
        idx++;
    }
    printf("\n");
}


/***********************************************
* Function name: main()
* Description: The main funciton for the program
* Parameters: Command line args
* Returns: Returns 0 upon succesful completion
************************************************/
int main(int argc, char* argv[])
{
    char* serverSockAddr[2];
    char* clientSockAddr[2];
    char clientHost[500];
    char port[500];
    char myPort[500];
    struct addrinfo sockinfo;
    int listenSockfd, newSock, clientSock;
    char msg[MESSAGE_MAX_SIZE];
    //char* msg;
    char fileName[FILENAME_MAX_SIZE];
    char* commands[6];

    // get socket address from command line
    if (argc < 2)
    {
        printf("Run ftserver with command \"./ftserver <port>\"\n");
        exit(1);
    }
    else
        strcpy(myPort, argv[1]);

    printf("Server running on %s\n", myPort);

	while(1)
	{
        printf("Waiting for connections\n");
        // listen and accept control connection
        //newSock = acceptClient(myPort);
        acceptClient(&newSock, myPort);
        printf("New client connected\n");

         // listen for response
        int bytes;
        memset(msg, '\0', MESSAGE_MAX_SIZE);
        bytes = recv(newSock, msg, MESSAGE_MAX_SIZE, 0);

        if (bytes == 0)
            exit(1);

        // parse received command into string array
        parseCommand(msg, commands);


        // if -l in client connection
        if (strcmp(commands[3], "-l") == 0)
        {
            // grab content from client
            strcpy(clientHost, commands[5]);
            strcpy(port, commands[4]);
            printf("Connection from %s.\n", clientHost);
            printf("List directory requested on port %s.\n", port);

            // send ACK over control to confirm everything is ok
            sendMessage("ACK", newSock);
            sleep(6);

            // Connect data connection
            // using hostname and port from control connection
            clientSock = connectToServer(clientHost, port);
            printf("Data connection to client established\n");
            sleep(3);

            // send directory contents
            memset(msg, '\0', MESSAGE_MAX_SIZE);
            getDirContents(msg);
            fflush(stdout);
            printf("Sending directory contents to %s:%s.\n", clientHost, port);
            sendMessage(msg, clientSock);
        }

        // elif -g in client connection
        else if (strcmp(commands[3], "-g") == 0)
        {
            // grab content from client
            strcpy(clientHost, commands[6]);
            strcpy(port, commands[5]);
            strcpy(fileName, commands[4]);
            printf("Connection from %s\n", clientHost);
            printf("File \"%s\" requested on port %s.\n", fileName, port);


            // if file exists
            if (fileExists(fileName))
            {
                // send ACK message to confirm everything is ok
                sendMessage("ACK", newSock);
                sleep(8);
                // Connect data connection
                // using hostname and port from control connection
                clientSock = connectToServer(clientHost, port);
                sleep(3);
                printf("Sending \"%s\" to %s:%s.\n", fileName, clientHost, port);
                // send file
                sendFileTxt(fileName, clientSock);
                printf("Transfer Complete\n");
            }
            else
            {
                // send no file found error
                printf("File not found. Sending error message to %s:%s.\n", clientHost, port);
                strcpy(msg, "ERROR: FILE NOT FOUND\n");
                sendMessage(msg, newSock);
            }
        }
        else
        {
            // send error
            strcpy(msg, "ERROR: Invalid command\n");
            sendMessage(msg, newSock);
        }
        close(clientSock);
        close(newSock);


	}
    return 0;
}
