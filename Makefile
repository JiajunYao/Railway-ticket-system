CLIENT_ROOT = Client
SERVER_ROOT = Server

all : ${CLIENT_ROOT}/client ${SERVER_ROOT}/server
.PHONY : all

# client part
${CLIENT_ROOT}/client : ${CLIENT_ROOT}/client.o ${CLIENT_ROOT}/client_connection.o client_server.o
	gcc -Wall -o ${CLIENT_ROOT}/client ${CLIENT_ROOT}/client.o ${CLIENT_ROOT}/client_connection.o client_server.o
${CLIENT_ROOT}/client.o : ${CLIENT_ROOT}/client.c
	gcc -Wall -o ${CLIENT_ROOT}/client.o -c ${CLIENT_ROOT}/client.c -D__DEBUG__
${CLIENT_ROOT}/client_connection.o : ${CLIENT_ROOT}/client_connection.c
	gcc -Wall -o ${CLIENT_ROOT}/client_connection.o -c ${CLIENT_ROOT}/client_connection.c -D__DEBUG__


# server part
${SERVER_ROOT}/server : ${SERVER_ROOT}/server.o ${SERVER_ROOT}/server_connection.o client_server.o
	gcc -Wall -o ${SERVER_ROOT}/server ${SERVER_ROOT}/server.o ${SERVER_ROOT}/server_connection.o client_server.o
${SERVER_ROOT}/server.o : ${SERVER_ROOT}/server.c
	gcc -Wall -o ${SERVER_ROOT}/server.o -c ${SERVER_ROOT}/server.c -D__DEBUG__
${SERVER_ROOT}/server_connection.o : ${SERVER_ROOT}/server_connection.c
	gcc -Wall -o ${SERVER_ROOT}/server_connection.o -c ${SERVER_ROOT}/server_connection.c -D__DEBUG__



# common part
client_server.o : client_server.c
	gcc -Wall -o client_server.o -c client_server.c -D__DEBUG__
