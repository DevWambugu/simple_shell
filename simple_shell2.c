#include "main.h"
#include <sys/stat.h>

/**
*get_path_of_command - returns the path location
*cmd: command given
*
*Return: command path string
*/
char *get_path_of_command(char *cmd)
{
	char *path;
	char *path_substrings;
	char *copy_of_the_path;
	int cmd_length;
	int path_substrings_length;
	char *file_path;
	struct stat buffer_path;

	path = getenv("PATH");
	if (path)
	{

		copy_of_the_path = strdup(path);
		cmd_length = strlen(cmd);
		path_substrings = strtok(copy_of_the_path, ":");
		while (path_substrings != NULL)
		{
			path_substrings_length = strlen(path_substrings);
			file_path = malloc(cmd_length + path_substrings_length + 2);
			strcpy(file_path, path_substrings);
			strcat(file_path, "/");
			strcat(file_path, cmd);
		
			if (stat(file_path, &buffer_path) == 0)
			{
				free(copy_of_the_path);
				return(file_path);
			}
			else
			{
				free(file_path);
				path_substrings = strtok(NULL, ":");
			}
		}
		free(copy_of_the_path);
		if (stat(cmd, &buffer_path) == 0)
		{
			return (cmd);
		}
		return (NULL);
	}
	return (NULL);
}

/**
*execute_command - executes the given command
*argv: argument vector
*
*Return: This is a void function
*/
void execute_command(char *argv[])
{
	char *cmd = argv[0];
	char *given_command = NULL;

	if (argv)
	{
		given_command = get_path_of_command(cmd);
		if (execve(given_command, argv, NULL) == -1)
		{
			perror("error while executing command");
			return;
		}
	}
	free(given_command);
}
/**
*main - runs a simple shell
*argc: argument count
*argv: argument vector
*
*Return: Always return 0
*/
int main(int argc, char *argv[])
{
	const char *prompt = "#cisfun$ ";
	char *lineptr = NULL;
	size_t buffer_size = 0;
	ssize_t characters_read;
	char *delimiters = " \n";
	char *line_pointer_copy = NULL;
	int number_of_substrings = 0;
	char *substrings;
	int i = 0;
	(void)argc;
	while (1)
	{
		write(STDOUT_FILENO, prompt, strlen(prompt));
		characters_read = getline(&lineptr, &buffer_size, stdin);
		if (characters_read == -1)
		{
			write(STDOUT_FILENO, "Exiting the shell.\n", strlen("Exiting the shell.\n"));
			return (-1);
		}
		line_pointer_copy = malloc(sizeof(char) * (characters_read + 1));
		if (line_pointer_copy == NULL)
		{
			return (0);
		}
		strcpy(line_pointer_copy, lineptr);
		substrings = strtok(lineptr, delimiters);
		while (substrings != NULL)
		{
			number_of_substrings++;
			substrings = strtok(NULL, delimiters);
		}
		number_of_substrings++;
		argv = malloc(sizeof(char *) * (number_of_substrings + 1));
		if (argv == NULL)
		{
			return (0);
		}
		substrings = strtok(line_pointer_copy, delimiters);
		while (substrings != NULL)
		{
			argv[i] = malloc(sizeof(char) * strlen(substrings));
			strcpy(argv[i], substrings);
			substrings = strtok(NULL, delimiters);
			i++;
		}
		argv[i] = NULL;
		execute_command(argv);
	}
	free(lineptr);
	free(line_pointer_copy);
	return (0);
}
