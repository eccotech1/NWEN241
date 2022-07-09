#include <stdio.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 

/**
 * Prints error message
 * @param *msg given message to be printed
 */ 
void error(const char *msg) {
    printf("Error: %s\n", msg);
}

/**
 * Create an endpoint for communication
 * @return int which is socket file descriptor of server
 */ 
int createSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0)
        error("creating socket");
    return sockfd;
}

/**
 * Bind the socket to an address
 * @param portNum port number to connect to
 * @param sockfd socket file descriptor of server which is socket to bind to
 */ 
void binding(int portNum, int sockfd) {
    struct sockaddr_in serverAddr; // define IP address and port number
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNum);

    if (bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
        error("binding");
}

/**
 * Listen for connections
 * @param sockfd socket file descriptor of server which is node to listen to
 */ 
void listening(int sockfd) {
    if (listen(sockfd, SOMAXCONN) < 0) // set pending connections to max number allowed by machine
        error("listening");
}

/**
 * Accept connection requests from a client
 * @param sockfd socket file descriptor of server
 * @return non-negative socket file descriptor of client
 */ 
int acceptConnection(int sockfd) {
    struct sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    int clientfd = accept(sockfd, (struct sockaddr*) &clientAddr, (socklen_t*) &clientAddr);

    if (clientfd < 0)
        error("accepting client");

    return clientfd;
}

/**
 * Send message to client and handle consequences of it failing
 * @param *msg error message to be displayed upon failure
 * @param sockfd socket file descriptor of server
 * @param clientfd socket file descriptor of client
 * @param *buf message to be sent to client
 */ 
void sendToClient(const char *msg, int sockfd, int clientfd, const void *buf) {
    ssize_t toSend = send(clientfd, buf, strlen(buf), 0);

    if(toSend < 0) { // upon unsuccessful sending of message
        error(msg);
        close(clientfd);
        close(sockfd);
        exit(0);
    }
}

/**
 * Read contents from file
 * @param *fileName name of file to be read
 * @param sockfd socket file descriptor of server
 * @param clientfd socket file descriptor of client
 * @param *incoming message received from client
 */ 
void readFromFile(const char *fileName, int sockfd, int clientfd, char *incoming) {
	FILE *in = fopen(fileName, "r");				
	if (in == NULL)
		sendToClient("file reading", sockfd, clientfd, "SERVER 404 Not Found\n");
	else {
		memset(incoming, '\0', sizeof(incoming)); // clear array
		sendToClient("file reading success message", sockfd, clientfd, "SERVER 200 OK\n\n");
		int lineSize = 50;
		char line[lineSize]; 
		
		while(fgets(line, lineSize, in)) { // read each line
			int spacePos = 0;
			while (line[spacePos] != '\0') { // find position after last character to append new line character
				spacePos++;
			}
			line[spacePos+1] = '\n';

			sendToClient("displaying file contents", sockfd, clientfd, line);
		}
		char newLineChar[] = {'\n','\n'}; // add 2 new lines to end of message
		sendToClient("appending new line after file contents", sockfd, clientfd, newLineChar);
	
		fclose(in);
	}
}

/**
 * Write contents to file
 * @param *out file to be written to
 * @param sockfd socket file descriptor of server
 * @param clientfd socket file descriptor of client
 * @param readingPos position after file name in incoming array
 * @param *incoming message received from client
 * @param *putIncoming message received from client to be written within PUT command
 */
void writeToFile(FILE *out, int sockfd, int clientfd, int readingPos, char *incoming, char *putIncoming) {				
	int foundNewLine = 0;
	int putTerminate = 0;
	while (putTerminate == 0) { // read until 2 empty lines found
		memset(putIncoming, '\0', sizeof(putIncoming)); // clear array

		ssize_t receivingPut = recv(clientfd, putIncoming, sizeof(putIncoming), 0); // get message from client
		if(receivingPut <= 0) {
			error("receiving PUT message");
			close(clientfd);				
			exit(0);
		}
		
		for (int i = 0; i < sizeof(putIncoming); i++) {
			if (putIncoming[i] != '\0') {
				if (putIncoming[0] == '\n') { // find new line char
					foundNewLine++;
				} else {
					foundNewLine = 0;
				}

				fprintf(out, "%c", putIncoming[i]);
				if (foundNewLine == 2) { // close file if 2 empty lines found
					fclose(out);
					sendToClient("file writing", sockfd, clientfd, "SERVER 201 Created\n");
					memset(putIncoming, '\0', sizeof(putIncoming)); // clear array
					putTerminate = 1; // stop while loop
				}
			} 
		}
	}				
}

int main(int argc, char *argv[])
{
    int portNum;
    char incoming[500];
    memset(incoming, '\0', sizeof(incoming)); // clear array

    char putIncoming[500];
    memset(putIncoming, '\0', sizeof(putIncoming)); // clear array

    if (argc < 2) // return -1 if no port number given
        exit(-1);

    sscanf(argv[1], "%d", &portNum); // convert argument to int
    if (portNum < 1024)
        exit(-1);

    int sockfd = createSocket(); // Step 1: make socket

    binding(portNum, sockfd); // Step 2: bind socket

    listening(sockfd); // Step 3: listen for connection

    printf("%s\n", "Starting server...");
	while (1) {		
        int clientfd = acceptConnection(sockfd); // Step 4: accept connection
        sendToClient("sending initial message", sockfd, clientfd, "HELLO\n");

		int terminate = 0;
		while (terminate == 0) {
			memset(incoming, '\0', sizeof(incoming)); // clear array

			ssize_t receiving = recv(clientfd, incoming, sizeof(incoming), 0); // get message from client

			if(receiving <= 0) {
				error("receiving message");
				close(clientfd);				
				exit(0);
			} 

			if ((incoming[0] == 'B' || incoming[0] == 'b') && (incoming[1] == 'Y' || incoming[1] == 'y') && (incoming[2] == 'E' || incoming[2] == 'e')) { // check if message contains BYE	
				printf("Received message: %s\n", "BYE");	
				printf("%s\n", "Closing connection with client");	
				close(clientfd); // close connection
				terminate = 1;
			} else if ((incoming[0] == 'G' || incoming[0] == 'g') && (incoming[1] == 'E' || incoming[1] == 'e') && (incoming[2] == 'T' || incoming[2] == 't')) { // check if message contains GET
				printf("Received message: %s", incoming);				
				char fileName[100];
				int fileNameFound = 0;
				memset(fileName, '\0', sizeof(fileName)); // clear array
				
				if (incoming[3] == ' ') { // find and store file name					
					int i=4;
					while(incoming[i] != '\0' && incoming[i] != ' '){							
						fileName[i-4] = incoming[i];
						i++;
					}
					fileName[i-5] = '\0';
					fileNameFound = 1;
				} 
				
				if (fileNameFound == 1) {
					readFromFile(fileName, sockfd, clientfd, incoming); // read from file
				} else {
					sendToClient("no file found", sockfd, clientfd, "SERVER 500 Get Error\n"); // error if no file name
				}
			} else if ((incoming[0] == 'P' || incoming[0] == 'p') && (incoming[1] == 'U' || incoming[1] == 'u') && (incoming[2] == 'T' || incoming[2] == 't')) { // check if message contains PUT
				printf("Received message: %s", incoming);	
				char fileName[100];
				int fileNameFound = 0;
				memset(fileName, '\0', sizeof(fileName)); // clear array
				int readingPos = 0; // position of char after file name

				if (incoming[3] == ' ') { // find and store file name					
					int i=4;
					while (incoming[i] != '\0' && incoming[i] != ' ' && incoming[i] != '\n') {							
						fileName[i-4] = incoming[i];
						i++;
						readingPos = i+1; // account for space after file name
					}
					fileName[i-4] = '\0';
					fileNameFound = 1;
				} 

				if (fileNameFound == 1) {
					FILE *out = fopen(fileName, "w");
					if (out == NULL)
						sendToClient("file writing", sockfd, clientfd, "SERVER 501 Put Error\n");
					else {
						for (int i = readingPos; i < sizeof(incoming); i++) {
							if (incoming[i] != '\0') {
								if (!(i == 0 && incoming[i] == '\n')) { // do not print new line if command line empty
									fprintf(out, "%c", incoming[i]); // write contents from line with PUT command to file
								} 
							}  
						}
						writeToFile(out, sockfd, clientfd, readingPos, incoming, putIncoming); // write to file
					}
				} else
				 	sendToClient("no file found", sockfd, clientfd, "SERVER 502 Command Error\n"); // error if no file name
			} else {
					sendToClient("no file found", sockfd, clientfd, "SERVER 502 Command Error\n"); // error if new line follows any other command	
			}
		}
   	}
    return 0;
}