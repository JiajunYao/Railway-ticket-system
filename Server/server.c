/**
 * This file is the implementaion of server
 */
#include "../client_server.h"
#include "server.h"

int fifo_server_run();
int socket_server_run();
void sig_chld(int signo);

int main()
{
	if(socket_server_run() == -1)
	{
		fprintf(stderr, "server has error\n");
		return -1;
	}

	return 0;
}

int fifo_server_run()
{
	FILE* read_file;
	if(create_listening_fifo(SERVER_PUBLIC_FIFO_NAME, &read_file) == -1)
	{
		fprintf(stderr, "can't create listening fifo\n");
		return -1;
	}
	
	#ifdef __DEBUG__
	fprintf(stdout, "start listening\n");
	#endif

	// listening to client establish connection request 
	char content[BUFFER_SIZE];
	char* fgets_result;
	pid_t client_pid;
	
	if(signal(SIGCHLD, sig_chld) == SIG_ERR)
	{
		fprintf(stderr, "can't bind signal handler\n");
		return -1;
	}

	while(true)
	{
		fgets_result = Fgets(content, sizeof(content), read_file);
		if(fgets_result == NULL)
		{
			fprintf(stderr, "error happens when listening connection\n");
			return -1;
		}
		else
		{
			if(atoi(content) != ESTABLISH_CONNECTION_REQUEST)
			{
				fprintf(stderr, "expect a ESTABLISH_CONNECTION_REQUEST but get %s\n", content);
				exit(-1);
			}
			if(read_line(content, sizeof(content), read_file, false) == NULL)
			{
				fprintf(stderr, "can't get client pid\n");
				return -1;
			}
			client_pid = atoi(content);

			if(fork() == 0) // child process
			{
				if(close_listening_fifo(read_file) == -1)
				{
					fprintf(stderr, "server child close listening fifo error\n");
					return -1;
				}

				FILE* write_file;
				if(establish_client_server_fifo_connection(client_pid, &read_file, &write_file) == -1)				
				{
					fprintf(stderr, "can't establish client server fifo connection\n");
					return -1;
				}
				#ifdef __DEBUG__
				fprintf(stdout, "establish client server fifo connection\n");
				#endif
				
				if(run_server_core(read_file, write_file) == -1)
				{
					fprintf(stderr, "server core has error\n");
					return -1;
				}
				
				
				// disconnect client
				if(fclose(write_file) == EOF)
				{
					fprintf(stderr, "close the write file occurs an error\n");
					return -1;
				}
				if(fclose(read_file) == EOF)
				{
					fprintf(stderr, "close the read file occurs an error\n");
					return -1;
				}

				return 0;
			}
		}
	}
}

int socket_server_run()
{
	int listen_fd, connect_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;

	if((listen_fd = create_listening_socket()) == -1)
	{
		fprintf(stderr, "can't create listening socket\n");
		return -1;
	}

	#ifdef __DEBUG__
	fprintf(stdout, "start listening\n");
	#endif

	if(signal(SIGCHLD, sig_chld) == SIG_ERR)
	{
		fprintf(stderr, "can't bind signal handler\n");
		return -1;
	}
	
	while(true)
	{
		client_len = sizeof(client_addr);
		if((connect_fd = Accept(listen_fd, (SA*)&client_addr, &client_len)) == -1)
		{
			fprintf(stderr, "accept error\n");
			return -1;
		}

		if(fork() == 0) // child process
		{
			FILE* read_file;
			FILE* write_file;

			if(close(listen_fd) == -1)
			{
				fprintf(stderr, "server child close listening socket error\n");
				return -1;
			}
			
			if((read_file = fdopen(connect_fd, "r+")) == NULL)
			{
				fprintf(stderr, "convertion from connected socket fd to FILE struct has error\n");
				return -1;
			}

			write_file = read_file; // socket is duplex

			#ifdef __DEBUG__
			fprintf(stdout, "establish client server socket connection\n");
			#endif

			if(run_server_core(read_file, write_file) == -1)
			{
				fprintf(stderr, "server core has error\n");
				return -1;
			}
			
			//disconnect client socket is duplex, so read_file and write_file is the same file
			if(fclose(read_file) == EOF)
			{
				fprintf(stderr, "close the read file occurs an error\n");
				return -1;
			}

			return 0;
		}

		if(close(connect_fd) == -1)
		{
			fprintf(stderr, "server close connected socket error\n");
			return -1;
		}
	}
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		#ifdef __DEBUG__
		printf("child %d terminated\n", pid);
		#endif
	}
	
	return;
}
