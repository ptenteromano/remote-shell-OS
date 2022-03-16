/* Philip Tenteromano
 * Server file
 *
 * Compile with 'make server' or 'make all'
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

// port number can be anything ( > 3000 usually)
#define PORT 5001;

// fork, exec, pipe function with cmd argument
std::string exec(const char*);

int main(int argc, char *argv[]) {

    // inital send data
    char* connMsg = "Server Connected!\n";

    // declare sockets, server info, buffer, response value, # of connects
    int serverSock;
    int newConnSock;
    sockaddr_in server;
    char buffer[1024];
    int rval;
    const int CONNECTIONS = 5;

    // initalize server socket
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // intialize server information, set to any form of internet / localhost and port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = PORT;

    // bind the server socket to localhost server (and also our port 5001)
    if (bind(serverSock, (sockaddr *)&server, sizeof(server))) {
        perror("Bind Failed");
        exit(1);
    }

    // listen on the bound socket, with up to 5 connections (non-functional at the moment)
    listen(serverSock, CONNECTIONS);
    printf("Server Booted Up!\n\n");

    timeval sessionStart, sessionEnd, recStart, recEnd;
    float sessTime, recTime, sessThroughput, recThroughput;
    int bytesMessage = 0, bytesSession = 0;

    // sit and listen - accept connections
    do {
        bool startUp = true; // initial connection bool
        newConnSock = accept(serverSock, (sockaddr *) 0, 0);
        if (newConnSock == -1)
            perror("Accept failed");
        else {
            // if client connects, send confirmation message, listen for data
            if(send(newConnSock, connMsg, strlen(connMsg), 0) < 0) {
                perror("Send failed");
                close(serverSock);
                exit(1);
            }

            // reset session data
            bytesSession = 0;
            gettimeofday(&sessionStart, NULL);

            // loop through receiving data from client
            do {
                bytesMessage = 0;
                // reset server incoming memory to buffer
                memset(&buffer, 0, sizeof(buffer));

                // receive and error check
                if ((rval = recv(newConnSock, &buffer, sizeof(buffer), 0)) < 0)
                    perror("Reading stream message error");
                else if (rval == 0) {
                    printf("Ending connection... \n");
                    break;
                }
                else {
                    // mark start time for connection time
                    gettimeofday(&recStart, NULL);
                    printf("New Message from client: %s\n", buffer);
                    // store bytes
                    bytesMessage += rval;
                }

                // if input is 'exit', send client close info and break out of loop
                if (strcmp(buffer, "exit") == 0) {
                    send(newConnSock, buffer, sizeof(buffer), 0);
                    break;
                }

                // on first run, don't attempt to exec anything
                if (startUp) {
                    startUp = false;
                    continue;
                }

                // call the forking, exec, and piping function
                std::string output = "";
                try {
                    output = exec(buffer);
                }
                catch(...) {
                    output = "Not Found\n";
                    perror("Exec failed");
                }

                // send the output of the exec call and error check
                if ((rval = send(newConnSock, (const void *) output.c_str(), output.length(), 0)) < 0)
                    perror("Exec send failed");
                else {
                    // store send bytes
                    bytesMessage += rval;
                    printf("Response Sent to Client!\n\n");
                }
                bytesSession += bytesMessage;

                // clock end of connection time, calculate and print to server console
                gettimeofday(&recEnd, NULL);
                recTime = (float) (recEnd.tv_usec - recStart.tv_usec) / (float) 1000;
                recThroughput = (float) bytesSession / recTime;

                printf("Message Latency: %.2f ms\n", recTime);
                printf("Bytes Transfered: %d B\n", bytesMessage);
                printf("Throughput of last connection: %.2f Bytes/sec \n\n", recThroughput);

            } while(1);

            // close client socket, print session time, calc session throughput
            close(newConnSock);
            gettimeofday(&sessionEnd, NULL);
            sessTime = (float) (sessionEnd.tv_usec - sessionStart.tv_usec) / (float) 1000;
            sessThroughput = (float) bytesSession / sessTime;

            printf("Session Time: %.2f ms\n", sessTime);
            printf("Total Session Bytes Transfered: %d B\n", bytesSession);
            printf("Session Throughput: %.2f Bytes/Sec \n\n", sessThroughput);

            printf("Socket closed, listening for more connections... \n");
        }
    // continue listening for new connections
    } while(1);

    return 0;
}

// function opens and writes to a file to pipe contents of an exec call
std::string exec(const char* cmd) {
    char buffer[1024];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        result += " ";
        while (!feof(pipe)) {
            if (fgets(buffer, 1024, pipe) != NULL)
                result += buffer;
        }
    }
    catch (...) {
        pclose(pipe);
        result = "Not Found";
        throw;
    }

    pclose(pipe);

    return result;
}
