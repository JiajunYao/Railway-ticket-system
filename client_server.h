/**
 * This file contains the data, macro common to both client and server
 */

#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 5001
#define LISTEN_BACKLOG 50
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PUBLIC_FIFO_NAME "/tmp/server_public_fifo"
#define SA struct sockaddr

#define SUCCESS 0
#define FAILURE 1
#define ESTABLISH_CONNECTION_REQUEST 2
#define ESTABLISH_CONNECTION_RESPONSE 3
#define ESTABLISH_CONNECTION_ACK 4
#define REGISTER_REQUEST 5
#define REGISTER_RESPONSE 6

char* Fgets(char* s, int size, FILE* stream);
ssize_t Write(int fd, const void *vptr, size_t n);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
#endif
