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
void create_and_write_file(char **args)
{
	int i = 0;
	char *file_content = NULL;
	char *file_name = NULL;
	int file_descriptor;
	ssize_t bytes_written;

	while (args[i] != NULL)
	{
		if (strcmp(args[i], ">>") == 0)
		{
			break;
		}
		i++;
	}
	printf("%d", i);
	if (args[i + 1] == NULL)
	{
		write(STDERR_FILENO, "Invalid input. Please enter file name\n", strlen("Invalid input. Please enter file name\n"));
		exit(1);
	}
	file_content = args[1];
	file_name = args[3];
	file_descriptor = open(file_name, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (file_descriptor == -1)
	{
		write(STDERR_FILENO, "could not open file\n", strlen("could not open file\n"));
		exit(1);
	}
	bytes_written = write(file_descriptor, file_content, strlen(file_content));
	if (bytes_written == -1)
	{
		write(STDERR_FILENO, "could not write file\n", strlen("could not write file\n"));
		return;
	}
	close(file_descriptor);
}

int main(void)
{
	const char *prompt = "#cisfun$ ";
	char *input;
	char **sub_strings;
	/**int max_processes = 10;*/
	int status;
	int i = 1;
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
				if (strcmp(sub_strings[0], "echo") == 0 && strcmp(sub_strings[2], ">>") == 0)
				{
					create_and_write_file(sub_strings);
				}
				else if (strcmp(sub_strings[0], "echo") == 0 && sub_strings[2] == NULL)
				{
					write(STDOUT_FILENO, sub_strings[1], strlen(sub_strings[1]));
					write(STDOUT_FILENO, "\n", 1);
					exit(1);
				}
				else if (strcmp(sub_strings[0], "echo") == 0 && strcmp(sub_strings[1], ">>") != 0)
				{
					while ((sub_strings[i]) != NULL)
					{
						write(STDOUT_FILENO, sub_strings[i], strlen(sub_strings[i]));
						write(STDOUT_FILENO, " ", 1);
						i++;
					}
					write(STDOUT_FILENO, "\n", 1);
					exit(1);
				}
				else
				{
					execvp(sub_strings[0], sub_strings);
					write(STDERR_FILENO, "Command not executed. Enter a valid command\n",
						strlen("Command not executed. Enter a valid command\n"));
					exit(1);
				}
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
