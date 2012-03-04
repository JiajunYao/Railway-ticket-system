#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>

int connect_fifo_server(char* server_public_fifo_name, FILE** read_file, FILE** write_file);
int disconnect_fifo_server(FILE** read_file, FILE** write_file);

#endif
