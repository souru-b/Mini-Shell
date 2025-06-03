#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include<error.h>
#include<errno.h>
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\033[1m"
#define ANSI_BOLD_RESET    "\033[0m"

typedef enum
{
	stop,
	run
}state;

typedef struct node
{
	pid_t pid;
	char str[50];
	state pstate;
	struct node *link;
}Slist;

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
int check_command_type(char *command);
void execute_internal_commands(char *input_string);
void extract_external_commands(char **external_commands);
int check_input_PS1(char *input, char *prompt);
void execute_external_commands(char *input);
void convert_2D_array(char *input, char **ext_cmd);
void signal_handler(int signum);
void print_list(Slist *head);
int insert_at_first(Slist **head, pid_t pid, char *input);
int sl_delete_first(Slist **head);

#endif
