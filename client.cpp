/* Philip Tenteromano
 * 
 * 12/13/2018
 * Operating Systems
 * Final Project
 * Remote Shell
 * 
 * Client file
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>

#define PORT 5001;

int main(int argc, char *argv[]) {

    // declare send data, socket, server info, localhost, and buffer
    char* data = "Client Connected!";
	int clientSock;
	sockaddr_in server; 
	hostent *hp;
	char buffer[1024];
    int rval;

    // initialize socket, check for error
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock < 0) {
	    perror("socket failed");
	    exit(1);
	}
	server.sin_family = AF_INET;

    // localhost:5001 ... or we can use `argv[1]` for variable address
	hp = gethostbyname("localhost");
	if(hp == 0) {
        perror("gethostbyname failed");
        close(clientSock);
        exit(1);
    }

    // set memory to copy of the server initial state
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = PORT;

    // connect and error check
	if(connect(clientSock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connect failed");
        close(clientSock);
        exit(1);
    }

    // set an input string, init to welcome message
    std::string line = data;            
    do {
        // send data over to server, convert input 'line' to c-style string
        if(send(clientSock, line.c_str(), strlen(line.c_str()), 0) < 0) {
                perror("send failed");
                close(clientSock);
                exit(1);
        }

        // exit loop and close socket with input == 'exit'
        if (strcmp(line.c_str(), "exit") == 0) {
            printf("Connection canceled!\n");
            send(clientSock, line.c_str(), sizeof(line.c_str()), 0);
            break;
        }

        // receive and error check
        if ((rval = recv(clientSock, &buffer, sizeof(buffer), 0)) < 0) 
            perror("Reading stream message error");
        else if (rval == 0) {
            printf("No Response, continuing... \n");
        }
        else 
            printf("New Message from Server: \n\n%s", buffer); 

        printf("\nMessage complete\n");
        
        // interactive client input (continuously input and send data)
        getline(std::cin, line);
        // reset buffer
        memset(&buffer, 0, sizeof(buffer)); 
        // store line into buffer
        strcpy(buffer, line.c_str());       

    } while(1);

    // terminate client connection and exit client process
    close(clientSock);

    return 0;
}
