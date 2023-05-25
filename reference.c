#include "main.h"

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
                        perror("read");
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

char *get_full_path(char *command)
{
        char *path = getenv("PATH");
        char *path_copy = strdup(path);
        char *dir;
        char *full_path;
        size_t dir_len = 0;
        size_t command_len = strlen(command);
        size_t full_path_len;

        dir = strtok(path_copy, ":");

        while (dir != NULL)
        {
                dir_len = strlen(dir);
                full_path_len = dir_len + 1 + command_len + 1;
                full_path = malloc(full_path_len);
                if (full_path == NULL)
                {
                        free(path_copy);
                        return (NULL);
                }
                write_path(full_path, dir, dir_len, command, command_len);
                if (access(full_path, F_OK) == 0)
                {
                        free(path_copy);
                        return (full_path);
                }
                free(full_path);
                dir = strtok(NULL, ":");
        }

        free(path_copy);
        return (NULL);
}

void write_path(char *full_path, const char *dir, size_t dir_len, const char *command, size_t command_len)
{
        memcpy(full_path, dir, dir_len);
        full_path[dir_len] = '/';
        memcpy(full_path + dir_len + 1, command, command_len + 1);
}

void execute_command(char **args)
{

        pid_t pid = fork();
        extern char **environ;
        char **env = environ;
        char *full_path = get_full_path(args[0]);

        if (pid == -1)
        {
                perror("fork");
                return;
        }
        else if (pid == 0)
        {
                if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "\"exit\"") == 0)
                {
                        write(STDOUT_FILENO, "Exiting the shell.\n", strlen("Exiting the shell.\n"));
                        exit(0);
                }
                else if (strcmp(args[0], "env") == 0)
                {
                        while (*env != NULL)
                        {
                                write(STDOUT_FILENO, *env, strlen(*env));
                                write(STDOUT_FILENO, "\n", 1);
                                env++;
                        }
                        exit(0);
                }
                else
                {
                        if (full_path != NULL)
                        {
                                execv(full_path, args);
                                perror("excevp");
                                exit(1);
                        }
                        else
                        {
                                write(STDERR_FILENO, "Command not found: ", strlen("Command not found: "));
                                write(STDERR_FILENO, args[0], strlen(args[0]));
                                write(STDERR_FILENO, "\n", 1);
                                exit(1);
                        }
                }
        }
        else
        {
                waitpid(pid, NULL, 0);
        }
}

/*void create_and_write_file(char **args)
{
        int i = 1;
        char *file_content = NULL;
        char *file_name = NULL;
        int file_descriptor;
        ssize_t bytes_written;

        while (args[i] != NULL)
        {
                if (strcmp(args[i], ">>") == 0)
                {
                        file_name = args[i + 1];
			break;
                }
		else
		{
			file_content = args[i + 1];

                i++;
        }

        if (args[i + 1] == NULL || args[i + 2] == NULL)
        {
                write(STDERR_FILENO, "Invalid input. Please enter file name\n", strlen("Invalid input. Please enter file name\n"));
                exit(1);
        }
        file_content = args[i + 1];
        file_name = args[i + 2];
        file_descriptor = open(file_name, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
        if (file_descriptor == -1)
        {
                write(STDERR_FILENO, "Could not open file\n", strlen("Could not open file\n"));
                exit(1);
        }
        bytes_written = write(file_descriptor, file_content, strlen(file_content));
        if (bytes_written == -1)
        {
                write(STDERR_FILENO, "Could not write file\n", strlen("Could not write file\n"));
                exit(1);
        }
        close(file_descriptor);
}*/

int main(void)
{
        const char *prompt = "#cisfun$ ";
        char *input;
        char **sub_strings;
        char *string;
	int status;
        int i = 1;
	char *file_name = NULL;
	int file_descriptor;
	int dup_result;
	int no_length = getpid();
	char numbers_array[20];

        while (1)
        {
                write(STDOUT_FILENO, prompt, strlen(prompt));
                input = read_input();
                sub_strings = split_input(input);
		
                if (strcmp(sub_strings[0], "exit") == 0 && (sub_strings[1] ==  NULL))
		{
			write(STDOUT_FILENO, "Exiting the shell.\n", strlen("Exiting the shell.\n"));
			break;
		}
		if (strcmp(sub_strings[0], "env") == 0 && (sub_strings[1] == NULL))
		{
			print_env();
		}
		if (strcmp(sub_strings[0], "#") == 0)
		{
			continue;
		}
		if (sub_strings[0] != NULL)
		{
			pid_t pid = fork();

			if (pid == -1)
			{
				continue;
			}
			else if (pid == 0)
			{
				if (strcmp(sub_strings[0], "echo") == 0)
				{
					while (sub_strings[i] != NULL)
					{
						if (strcmp(sub_strings[i], ">>") == 0)
						{
							file_name = (sub_strings[i + 1]);
							if (file_name != NULL)
							{
								file_descriptor = open(file_name, O_WRONLY | O_APPEND |
									O_CREAT, S_IRUSR | S_IWUSR);
							}
							if (file_descriptor == -1)
							{
								write(STDERR_FILENO, "Could not open file\n",
									strlen("Could not open file\n"));
								exit(1);
							}
							dup_result = dup2(file_descriptor, STDOUT_FILENO);
							if (dup_result == -1)
							{
								perror("dupulicate_error");
								exit(1);
							}
							close(file_descriptor);
							break;
						}					
						else if (sub_strings[i][0] == '"' && sub_strings[i][strlen(sub_strings[i]) - 1] == '"')
						{
							string = sub_strings[i];
							string[strlen(sub_strings[i]) - 1] = '\0';
							write(STDOUT_FILENO, string + 1, strlen(string + 1));
						}
						else if (strcmp(sub_strings[1], "$$") == 0 && strcmp(sub_strings[2], "#") == 0)
						{
							sprintf(numbers_array, "%d", no_length);
							write(STDOUT_FILENO, numbers_array, strlen(numbers_array));
							break;
						}
						else
						{
							write(STDOUT_FILENO, sub_strings[i], strlen(sub_strings[i]));
						}
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
                        write(STDERR_FILENO, "Invalid input. Kindly provide valid input\n",
                        strlen("Invalid input. Kindly provide valid input\n"));
		}
                free(input);
                free(sub_strings);
                }
                return (0);
}
