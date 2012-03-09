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
static MYSQL_RES* res_set;
static MYSQL_ROW row;
static char content[BUFFER_SIZE];

int handle_finish_request();
int handle_register_request();
int handle_login_request();
int handle_query_by_station_request();
int handle_order_request();

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
			case QUERY_BY_STATION_REQUEST:
				if(handle_query_by_station_request() == -1)
				{
					fprintf(stderr, "handle query by station request error\n");
					return -1;
				}
				break;
			case ORDER_REQUEST:
				if(handle_order_request() == -1)
				{
					fprintf(stderr, "handle order request error\n");
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

	snprintf(content, sizeof(content), "SELECT id, password FROM client WHERE name = '%s'", name);
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
				if(strcmp(passwd, row[1]) == 0)
				{
					login_result = SUCCESS;
					//record current user
					current_client_id = atoi(row[0]);
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

int handle_query_by_station_request()
{

	#ifdef __DEBUG__
	printf("handle query by station request\n");
	#endif
	char start_station[MAX_STRING];
	char end_station[MAX_STRING];

	if(fgets(start_station, sizeof(start_station), read_file) == NULL)
	{
		fprintf(stderr, "can't get start station\n");
		return -1;
	}
	start_station[strlen(start_station) - 1] = '\0';

	if(fgets(end_station, sizeof(end_station), read_file) == NULL)
	{
		fprintf(stderr, "can't get end station\n");
		return -1;
	}
	end_station[strlen(end_station) - 1] = '\0';
	
	snprintf(content, sizeof(content), "SELECT t.name, sta1.name, sta2.name, ADDTIME(t.departure_time, SEC_TO_TIME(sch1.cost_time * 60)), ADDTIME(t.departure_time, SEC_TO_TIME(sch2.cost_time * 60)), sch2.cost_money - sch1.cost_money FROM schedule sch1, schedule sch2, station sta1, station sta2, train t WHERE sch1.train_id = sch2.train_id AND sch1.train_id = t.id AND sch1.station_id = sta1.id AND sch2.station_id = sta2.id AND sch1.cost_time < sch2.cost_time AND sta1.name = '%s' AND sta2.name = '%s'", start_station, end_station);

	if(mysql_query(db_conn, content) != 0)
	{
		print_mysql_error(db_conn, "can't retrieve satisfied train");
		return -1;
	}
	else
	{
		res_set = mysql_store_result(db_conn);
		if(res_set == NULL)
		{
			print_mysql_error(db_conn, "mysql_store_result() failed");
			return -1;
		}
		else
		{
			int result_number = mysql_num_rows(res_set);

			snprintf(content, sizeof(content), "%d\n%d\n", QUERY_BY_STATION_RESPONSE, result_number);
			Write(fileno(write_file), content, sizeof(char) * strlen(content));

			int i;
			for(i = 0; i < result_number; i++)
			{
				row = mysql_fetch_row(res_set);
				snprintf(content, sizeof(content), "%s\n%s\n%s\n%s\n%s\n\%s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
				Write(fileno(write_file), content, sizeof(char) * strlen(content));
			}
		}
	}

	return 0;
}

int handle_order_request()
{
	#ifdef __DEBUG__
	printf("handle query by station request\n");
	#endif


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
