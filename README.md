# Server-Client C++

## Features:
1. A remote shell used for simulate core concepts of OS system calls
2. server.cpp runs a process which listens on `localhost:5001`
3. client.cpp runs a process to look for that socket address
4. If the server is there and accepting, create a new socket connection
5. The connection stays open indefinitely, until the client inputs `exit`
6. The client reads input line by line from `stdin`
7. The client sends this data to the server, the server handles the processing
8. The server makes a pipe system call to create a child process and execute the input
9. The exec System call is looking to execute a bash command and store it in variable
10. Finally, the server will send back the contents of the output to the client
11. The client renders the output, and continues to sit on the connection


### How to run
1. Clone the repo
2. Run the following commands in the directory to compile
  * `g++ server.cpp -o server`
  * `g++ client.cpp -o client`
3. Open two separate terminals and run `server` in the first open
4. Once the server is running, run `client` in the other Once
5. Play around and enjoy!
