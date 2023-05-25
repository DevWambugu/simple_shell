#ifndef HEADER
#define HEADER
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]);
char *read_input(void);
char *get_path_of_command(char *cmd);
char **split_input(char *input);
char *get_full_path(char *command);
void write_path(char *full_path, const char *dir, size_t dir_len, const char *command, size_t command_len);
void create_and_write_file(char **args);
void print_env(void);
void execute_command(char *argv[]);
void print_env(void)
{
	extern char **environ;
	char **env = environ;

	while (*env != NULL)
	{
		write(1, *env, strlen(*env));
		write(1, "\n", 1);
		env++;
	}
}
#endif
