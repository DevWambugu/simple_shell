#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "main.h"
#define BUFFER_SIZE 1024

char **split_input(char *input)
{
	int l = 0;
	int c = 32;
	char **sub_strings;
	char *sub_string;
	char *delimiters = " \t\r\n";

	sub_strings = malloc(c * sizeof(char*));
	if (sub_strings == NULL)
	{
		return (NULL);
	}
	sub_string = strtok(input, delimiters);
	while (sub_string != NULL)
	{
		sub_strings[l] = sub_string;
		l++;
		sub_string = strtok(NULL, delimiters);
	}
	sub_strings[l] = NULL;
	return (sub_strings);
}
char *read_input(void)
{
    char *input = NULL;
    char buffer[BUFFER_SIZE];
    int i = 0, j = 0;
    int total_bytes = 0;
    int no_of_bytes_read;

	while (1)
	{
		no_of_bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
		if (no_of_bytes_read == -1)
		{
		exit(1);
		} 
		else if (no_of_bytes_read == 0)
		{
			break;
		}
		total_bytes += no_of_bytes_read;
		input = realloc(input, total_bytes + 1);
		if (input == NULL)
		{
			exit(1);
		}	
		for (j = 0; j < no_of_bytes_read; j++)
		{
			if (buffer[j] == '\n') 
			{
				input[i] = '\0';
				return (input);
			}
			else
			{
				input[i] = buffer[j];
				i++;
			}
		}
	}
	if (total_bytes > 0)
	{
		input[i] = '\0';
		return (input);
	}
	else
	{
		return NULL;
	}
}
int main(void)
{
	const char *prompt = "#cisfun$ ";
	char *input;
	char **sub_strings;
	/**int max_processes = 10;*/
	int status;
	/*int num_processes = 0;*/

	while (1)
	{
		write(STDOUT_FILENO, prompt, strlen(prompt));
		input = read_input();
		sub_strings = split_input(input);
		/**if (num_processes >= max_processes)
		{
			write(STDERR_FILENO, "Maximum number of child processes reached\n",
				strlen("Maximum number of child processes reached\n");
			break;
		}*/
		if (sub_strings[0] != NULL)
		{
			pid_t pid = fork();
			if (pid == -1)
			{
				continue;
			}
			else if (pid == 0)
			{
				execvp(sub_strings[0], sub_strings);
				write(STDERR_FILENO, "Command not executed\n",
					strlen("Command not executed\n"));
				exit(1);
			}
			else
			{
				waitpid(pid, &status,0);
			}
		}
		else
		{
			write(STDERR_FILENO, "invalid input. Kindly provide valid input\n",
				strlen("invalid input. Kindly provide valid input\n"));
		}
		free(input);
		free(sub_strings);
	}
	return (0);
}
