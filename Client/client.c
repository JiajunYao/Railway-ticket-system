/**
 * This file is the implementaion of client
 */
#include "../client_server.h"

int main()
{
	FILE* read_file, write_file;
	if(connect_fifo_server(SERVER_PUBLIC_FIFO_NAME , &read_file, &write_file) == -1)
	{
		fprintf(stderr, "connect_fifo_server has an error\n");
		exit(-1);
	}
	if(disconnect_fifo_server(read_file, write_file) == -1)
	{
		fprintf(stderr, "disconnect_fifo_server has an error\n");
		exit(-1);
	}

	return 0;
}
