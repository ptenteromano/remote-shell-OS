# simple Makefile to generate two programs
# `make all` to compile and get executables
# for remote shell 

all: server client

server: server.cpp
	g++ server.cpp -o server

client: client.cpp
	g++ client.cpp -o client
