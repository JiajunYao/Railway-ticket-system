/**
 * This file is the implementaion of server
 */
#include "../client_server.h"
#include "server.h"

int fifo_server_run();
void sig_chld(int signo);

int main()
{
	if(fifo_server_run() == -1)
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
		fgets_result = fgets(content, sizeof(content), read_file);
		if(fgets_result == NULL && !feof(read_file))
		{
			fprintf(stderr, "error happens when listening connection\n");
			return -1;
		}
		else if(fgets_result == NULL && feof(read_file))
		{
			continue;
		}
		else
		{
			if(atoi(content) != ESTABLISH_CONNECTION_REQUEST)
			{
				fprintf(stderr, "expect a ESTABLISH_CONNECTION_REQUEST but get %s\n", content);
				exit(-1);
			}
			if(fgets(content, sizeof(content), read_file) == NULL)
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

				return 0;
			}
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
		fprintf(stdout, "child %d terminated\n", pid);
		#endif
	}
	
	return;
}
