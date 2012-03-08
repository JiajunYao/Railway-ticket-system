#include <curses.h>
#include "../client_server.h"

#define QUIT 0
#define LOGIN 1
#define REGISTER 2

#define ERROR_COLOR 1

static FILE* read_file;
static FILE* write_file;

static int choice;
static char* login_menu[] = {"login", "register", "quit", 0};
static int login_command[] = {LOGIN, REGISTER, QUIT};
static char** current_menu = login_menu;
static int* current_command = login_command;
static char content[BUFFER_SIZE];

extern int LINES;
extern int COLS;

void show_welcome_interface();
int getchoice(char* greet, char* choices[], int commands[]);
void draw_menu(char* options[], int current_highlight, int start_y, int start_x);
int run_quit_module();
int run_login_module();
int run_register_module();
void get_string(char* string, int max_length);
void get_passwd(char* passwd, int max_length, int start_y, int start_x);
void show_error(int start_y, int start_x, char* message);

int run_client_core(FILE* read, FILE* write)
{
	// init the staic variable
	read_file = read;
	write_file = write;

	int client_core_result = 0;

	initscr();
	start_color();
	// init some color
	init_pair(ERROR_COLOR, COLOR_RED, COLOR_WHITE);	

	show_welcome_interface();
	do
	{
		choice = getchoice("login or register", current_menu, current_command); 
		switch (choice)
		{
			case QUIT:
				if(run_quit_module() == -1)
				{
					fprintf(stderr, "quit module has error\n");
					client_core_result = -1;
				}
				break;
			case LOGIN:
				if(run_login_module() == -1)
				{
					fprintf(stderr, "login module has error\n");
					client_core_result = -1;
				}
				break;
			case REGISTER:
				if(run_register_module() == -1)
				{
					fprintf(stderr, "register module has error\n");
					client_core_result = -1;
				}
				break;
		}
	} while (choice != QUIT && client_core_result != -1);

	endwin();

	return client_core_result;
}

void show_welcome_interface()
{
	
	const char welcome_slogan[] = "Welcome to";
	const char system_name[] = "Railway Ticket System";

	int middle_y = LINES   / 2;

	int start_x = (COLS - strlen(welcome_slogan)) / 2;
	int start_y = middle_y - 1;
	
	const char* scan_ptr;

	clear(); // clear the screen
	attron(A_DIM);
	scan_ptr = welcome_slogan + strlen(welcome_slogan) - 1;
	while(scan_ptr >= welcome_slogan)
	{
		move(start_y, start_x);
		insch(*(scan_ptr--));
		refresh();
	}
	
	start_x = (COLS - strlen(system_name)) / 2;
	start_y = middle_y + 1;

	scan_ptr = system_name + strlen(system_name) - 1;
	while(scan_ptr >= system_name)
	{
		move(start_y, start_x);
		insch(*(scan_ptr--));
		refresh();
	}

	attroff(A_DIM);
	sleep(1);
}

/*
 * this function is from Beginning Linux Programming and is modified
 * @param choices the string representation of all choices
 * @param commands the internal representation of all choices. choices and commands have a one-to-one relation
 */
int getchoice(char* greet, char* choices[], int commands[])
{
	static int selected_row = 0;

	int max_row = 0;
	char** option;
	option = choices;
	while(*option)
	{
		max_row++;
		option++;
	}

	int start_y = (LINES - max_row) / 2 + max_row;
	int start_x;
	int selected;
	int key = 0;

	if(selected_row >= max_row)
	{
		selected_row = 0;
	}

	clear();
	start_x = (COLS - strlen(greet)) / 2;
	mvprintw(start_y - 2, start_x, greet);
	
	keypad(stdscr, true);
	cbreak();
	noecho();
	key = 0;
	while(key != KEY_ENTER && key != '\n'){
		if(key == KEY_UP)
		{
			if(selected_row == 0)
			{
				selected_row = max_row - 1;
			}
			else
			{
				selected_row--;
			}
		}
		if(key == KEY_DOWN)
		{
			if(selected_row == (max_row - 1))
			{
				selected_row = 0;
			}
			else
			{
				selected_row++;
			}
		}
		selected = commands[selected_row];
		draw_menu(choices, selected_row, start_y, start_x);
		key = getch();
	}
	keypad(stdscr, false);
	nocbreak();
	echo();

	return selected;
}

void draw_menu(char* options[], int current_highlight, int start_y, int start_x)
{
	int current_row = 0;
	char** option_ptr;
	char* txt_ptr;
	option_ptr = options;
	while(*option_ptr)
	{
		if(current_row == current_highlight)
		{
			attron(A_STANDOUT);
		}
		txt_ptr = options[current_row];
		mvprintw(start_y + current_row, start_x, "%s", txt_ptr);
		if(current_row == current_highlight)
		{
			attroff(A_STANDOUT);
		}
		current_row++;
		option_ptr++;
	}

	mvprintw(start_y + current_row + 3, start_x, "Move highlight then press Return");
	refresh();
}

int run_quit_module()
{
	// notify the server
	snprintf(content, sizeof(content), "%d\n", FINISH_REQUEST);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));
	if(fgets(content, sizeof(content), read_file) == NULL)
	{
		fprintf(stderr, "can't get server quit ack\n");
		return -1;
	}
	if(atoi(content) != FINISH_ACK)
	{
		fprintf(stderr, "expect quit_ack but get %s\n", content);
		return -1;
	}

	return 0;
}

int run_login_module()
{
	char name[MAX_STRING];
	char passwd[MAX_STRING]; 
	char* name_hint = "Enter the user name:   ";
	char* passwd_hint = "Enter the password:    ";
	char* name_passwd_invalid = "User name and password are invalid";
	clear();

	// get user name
	int start_y = LINES / 2 + 1;
	int start_x = COLS / 2 - strlen(name_hint);
	mvprintw(start_y, start_x, "%s", name_hint);
	get_string(name, MAX_STRING);

	// get password
	start_y = start_y + 2;
	mvprintw(start_y, start_x, "%s", passwd_hint);
	get_passwd(passwd, sizeof(passwd), start_y, start_x + strlen(passwd_hint));	

	//verify at server
	snprintf(content, sizeof(content), "%d\n%s\n%s\n", LOGIN_REQUEST, name, passwd);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));
	if(fgets(content, sizeof(content), read_file) == NULL)
	{
		fprintf(stderr, "can't get login response\n");
		return -1;
	}
	if(atoi(content) != LOGIN_RESPONSE)
	{
		fprintf(stderr, "expect login_response but get %s\n", content);
		return -1;
	}
	if(fgets(content, sizeof(content), read_file) == NULL)
	{
		return -1;
	}
	if(atoi(content) == FAILURE)
	{
		start_y = start_y + 2;
		show_error(start_y, start_x, name_passwd_invalid);
		sleep(2);
	}
	else
	{
		// TODO reset menu
	}
	return 0;
}

int run_register_module()
{
	char name[MAX_STRING];
	char passwd[MAX_STRING];
	char confirm_passwd[MAX_STRING];
	char* name_hint = "Enter the user name:      ";
	char* passwd_hint = "Enter the password:       ";
	char* confirm_passwd_hint = "Enter the password again:  ";
	char* passwd_not_same = "Passwords are not same";
	char* user_exist = "This user name is not available";
	int start_y;
	int start_x;

	clear();
	// get user name
	start_y = LINES / 2 - 2;
	start_x = COLS / 2 - strlen(name_hint);
	mvprintw(start_y, start_x, "%s", name_hint);
	get_string(name, sizeof(name));

	// get password
	start_y = start_y + 2;
	mvprintw(start_y, start_x, "%s", passwd_hint);
	get_passwd(passwd, sizeof(passwd), start_y, start_x + strlen(passwd_hint));

	// get confirm passwd
	start_y = start_y + 2;
	mvprintw(start_y, start_x, "%s", confirm_passwd_hint);
	get_passwd(confirm_passwd, sizeof(confirm_passwd), start_y, start_x + strlen(confirm_passwd_hint));
	
	if(strcmp(passwd, confirm_passwd) != 0)
	{
		start_y = start_y + 2;
		show_error(start_y, start_x, passwd_not_same);
		sleep(2);
		return 0;
	}
	
	// update server
	snprintf(content, sizeof(content), "%d\n%s\n%s\n", REGISTER_REQUEST, name, passwd);
	Write(fileno(write_file), content, sizeof(char) * strlen(content));
	if(fgets(content, sizeof(content), read_file) == NULL)
	{
		fprintf(stderr, "can't get register response\n");
		return -1;
	}
	if(atoi(content) != REGISTER_RESPONSE)
	{
		return -1;
	}
	if(fgets(content, sizeof(content), read_file) == NULL)
	{
		return -1;
	}
	if(atoi(content) == FAILURE)
	{
		start_y = start_y + 2;
		show_error(start_y, start_x, user_exist);
		sleep(2);
	}

	return 0;
}

void get_string(char* string, int max_length)
{
	int len;
	wgetnstr(stdscr, string, max_length);
	len = strlen(string);
	if(len > 0 && string[len - 1] == '\n')
		string[len - 1] = '\0';
}

void get_passwd(char* passwd, int max_length, int start_y, int start_x)
{
	cbreak();
	noecho();
	int i = 0;
	while(i < max_length)
	{
		passwd[i] = getch();
		if(passwd[i] == '\n')
		{
			passwd[i] = '\0';
			break;
		}
		else if(passwd[i] == 127 || passwd[i] == 8)
		{
			if(i != 0)
			{
				i--;
				move(start_y, start_x + i);
				delch();
			}
		}
		else
		{
			move(start_y, start_x + i);
			addch('*');
			i++;
		}
		refresh();
	}
	passwd[max_length - 1] = '\0'; // defensive
	echo();
	nocbreak();
}

void show_error(int start_y, int start_x, char* message)
{
	attron(COLOR_PAIR(ERROR_COLOR));
	mvprintw(start_y, start_x, "%s", message);
	refresh();
	flash();
	attroff(COLOR_PAIR(ERROR_COLOR));
}
