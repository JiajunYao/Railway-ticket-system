#include <my_global.h>
#include <mysql.h>
#include <my_sys.h>
#include "../client_server.h"

#define DATABASE_USER_NAME "manager"
#define DATABASE_USER_PASSWD "manager"
#define DATABASE_NAME "railway_ticket_system"

static FILE* read_file;
static FILE* write_file;
static long int current_client_id;
static MYSQL* db_conn;
static char content[BUFFER_SIZE];

int handle_finish_request();
int handle_register_request();
int handle_login_request();
void print_mysql_error(MYSQL* conn, char* message);

int run_server_core(FILE* read, FILE* write)
{
	// init the static variable
	read_file = read;
	write_file = write;
	current_client_id = -1;
	
	// connect database
	db_conn = mysql_init(NULL);
	if(db_conn == NULL)
	{
		fprintf(stderr, "mysql_init() failed\n");
		return -1;
	}
	if(mysql_real_connect(db_conn, "localhost", DATABASE_USER_NAME, DATABASE_USER_PASSWD, DATABASE_NAME, 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "mysql_real_connect() failed\n");
		mysql_close(db_conn);
		return -1;
	}

	int request;
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
			case FINISH_REQUEST:
				if(handle_finish_request() == -1)
				{
					fprintf(stderr, "handle finish request error\n");
					return -1;
				}
				break;
			case REGISTER_REQUEST:
				if(handle_register_request() == -1)
				{
					fprintf(stderr, "handle register request error\n");
					return -1;
				}
				break;
			case LOGIN_REQUEST:
				if(handle_login_request() == -1)
				{
					fprintf(stderr, "handle login request error\n");
					return -1;
				}
				break;
			default:
				fprintf(stderr, "unknown request: %d\n", request);
				return -1;
		}
	} while(request != FINISH_REQUEST);

	mysql_close(db_conn);
	return 0;
}

int handle_finish_request()
{
	#ifdef __DEBUG__
	printf("handle finish request\n");
	#endif
	snprintf(content, sizeof(content), "%d\n", FINISH_ACK);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));

	return 0;
}

int handle_login_request()
{
	#ifdef __DEBUG__
	printf("handle login request\n");
	#endif
	char name[MAX_STRING];
	char passwd[MAX_STRING];
	int login_result;

	if(fgets(name, sizeof(name), read_file) == NULL)
	{
		fprintf(stderr, "can't get user name\n");
		return -1;
	}
	name[strlen(name) - 1] = '\0'; // remove the line break;

	if(fgets(passwd, sizeof(passwd), read_file) == NULL)
	{
		fprintf(stderr, "can't get user password\n");
		return -1;
	}
	passwd[strlen(passwd) - 1] = '\0';

	MYSQL_RES* res_set;
	MYSQL_ROW row;
	snprintf(content, sizeof(content), "SELECT password FROM client WHERE name = '%s'", name);
	if(mysql_query(db_conn, content) != 0)
	{
		print_mysql_error(db_conn, "can't retrieve user password");
		return -1;
	}
	else
	{
		res_set = mysql_store_result(db_conn);
		if(res_set == NULL)
		{
			print_mysql_error(db_conn, "mysql_store_result() failed");
			login_result = FAILURE;
		}
		else
		{
			if(mysql_num_rows(res_set) == 0)
			{
				login_result = FAILURE;
			}
			else
			{
				row = mysql_fetch_row(res_set);
				if(strcmp(passwd, row[0]) == 0)
				{
					login_result = SUCCESS;
				}
				else
				{
					login_result = FAILURE;
				}
			}
		}
		mysql_free_result(res_set);
    }

	// send response
	snprintf(content, sizeof(content), "%d\n%d\n", LOGIN_RESPONSE, login_result);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));

	return 0;
}

int handle_register_request()
{

	#ifdef __DEBUG__
	printf("handle register request\n");
	#endif
	char name[MAX_STRING];
	char passwd[MAX_STRING];
	int  register_result;

	if(fgets(name, sizeof(name), read_file) == NULL)
	{
		fprintf(stderr, "can't get user name\n");
		return -1;
	}
	name[strlen(name) - 1] = '\0';

	if(fgets(passwd, sizeof(passwd), read_file) == NULL)
	{
		fprintf(stderr, "can't get user password\n");
		return -1;
	}
	passwd[strlen(passwd) - 1] = '\0';

	snprintf(content, sizeof(content), "INSERT INTO client (name, password) VALUES ('%s', '%s')", name, passwd);
	if(mysql_query(db_conn, content) != 0)
	{
		print_mysql_error(db_conn, "can't create new user");
		register_result = FAILURE;
	}
	else
	{
		register_result = SUCCESS;
	}

	// send response
	snprintf(content, sizeof(content), "%d\n%d\n", REGISTER_RESPONSE, register_result);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));

	return 0;
}

// this function is a facility
void print_mysql_error(MYSQL* conn, char* message)
{
	fprintf(stderr, "%s\n", message);
	if(conn != NULL)
	{
	#if MYSQL_VERSION_ID >= 40101
		fprintf(stderr, "Error %u (%s): %s\n", mysql_errno(conn), mysql_sqlstate(conn), mysql_error(conn));
	#else
		fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
	#endif
	}
}
