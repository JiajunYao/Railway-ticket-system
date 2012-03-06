CLIENT_ROOT = Client
SERVER_ROOT = Server
DEBUG = -D__DEBUG__


all : ${CLIENT_ROOT}/client ${SERVER_ROOT}/server
.PHONY : all

# client part
${CLIENT_ROOT}/client : ${CLIENT_ROOT}/client.o ${CLIENT_ROOT}/client_connection.o ${CLIENT_ROOT}/client_core.o client_server.o
	gcc -Wall -o ${CLIENT_ROOT}/client ${CLIENT_ROOT}/client.o ${CLIENT_ROOT}/client_connection.o ${CLIENT_ROOT}/client_core.o client_server.o -lcurses
${CLIENT_ROOT}/client.o : ${CLIENT_ROOT}/client.c
	gcc -Wall -o ${CLIENT_ROOT}/client.o -c ${CLIENT_ROOT}/client.c ${DEBUG}
${CLIENT_ROOT}/client_connection.o : ${CLIENT_ROOT}/client_connection.c
	gcc -Wall -o ${CLIENT_ROOT}/client_connection.o -c ${CLIENT_ROOT}/client_connection.c ${DEBUG}
${CLIENT_ROOT}/client_core.o : ${CLIENT_ROOT}/client_core.c
	gcc -Wall -o ${CLIENT_ROOT}/client_core.o -c ${CLIENT_ROOT}/client_core.c ${DEBUG} -I/user/include

# server part
${SERVER_ROOT}/server : ${SERVER_ROOT}/server.o ${SERVER_ROOT}/server_connection.o client_server.o
	gcc -Wall -o ${SERVER_ROOT}/server ${SERVER_ROOT}/server.o ${SERVER_ROOT}/server_connection.o client_server.o
${SERVER_ROOT}/server.o : ${SERVER_ROOT}/server.c
	gcc -Wall -o ${SERVER_ROOT}/server.o -c ${SERVER_ROOT}/server.c ${DEBUG}
${SERVER_ROOT}/server_connection.o : ${SERVER_ROOT}/server_connection.c
	gcc -Wall -o ${SERVER_ROOT}/server_connection.o -c ${SERVER_ROOT}/server_connection.c ${DEBUG}



# common part
client_server.o : client_server.c
	gcc -Wall -o client_server.o -c client_server.c ${DEBUG}
