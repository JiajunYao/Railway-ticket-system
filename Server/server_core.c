#include "../client_server.h"

static FILE* read_file;
static FILE* write_file;
static long int current_client_id;

int handle_register_request();

int run_server_core(FILE* read, FILE* write)
{
	// init the static variable
	read_file = read;
	write_file = write;
	current_client_id = -1;

	int request;
	char content[BUFFER_SIZE];
	do
	{	
		errno = 0;
		if(fgets(content, sizeof(content), read_file) == NULL)
		{
			if(feof(read_file))
			{
				fprintf(stdout, "client close its connection abruptly\n");
				return 0;
			}
			else
			{
				fprintf(stderr, "can't get user request: %s\n", strerror(errno));
				return -1;
			}
		}
		request = atoi(content);
		switch (request)
		{
			case REGISTER_REQUEST:
				if(handle_register_request() == -1)
				{
					fprintf(stderr, "handle register request error\n");
					return -1;
				}
				break;
			default:
				fprintf(stderr, "unknown request: %d\n", request);
				return -1;
		}
	} while(request != FINISH_REQUEST);

	return 0;
}

int handle_register_request()
{

	#ifdef __DEBUG__
	printf("handle register request\n");
	#endif
	char content[BUFFER_SIZE];
	char name[MAX_STRING];
	char passwd[MAX_STRING];
	if(fgets(name, sizeof(name), read_file) == NULL)
	{
		fprintf(stderr, "can't get user name\n");
		return -1;
	}
	if(fgets(passwd, sizeof(passwd), read_file) == NULL)
	{
		fprintf(stderr, "can't get user password\n");
		return -1;
	}
	
	// TODO update database
	snprintf(content, sizeof(content), "%d\n%d\n", REGISTER_RESPONSE, FAILURE);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));

	return 0;
}
