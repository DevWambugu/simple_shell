#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "main.h"
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

char **split_input(char *input)
{
        int l = 0;
        int c = 32;
        char **sub_strings;
        char *sub_string;
        char *delimiters = " \t\r\n";
        char *quote = "\"";
        size_t sub_string_len;
        int in_quotes = 0;

        sub_strings = malloc(c * sizeof(char *));
        if (sub_strings == NULL)
        {
                perror("malloc");
                return (NULL);
        }
        sub_string = strtok(input, delimiters);
        while (sub_string != NULL)
        {
                if (sub_string[0] == quote[0] && !in_quotes)
                {
                        in_quotes = 1;
                        sub_string_len = strlen(sub_string);
                        if (sub_string_len > 1 && sub_string[sub_string_len - 1] == quote[0])
                        {
                                sub_string[sub_string_len - 1] = '\0';
                                sub_string++;
                                in_quotes = 0;
                        }
                }
                else if (sub_string[strlen(sub_string) - 1] == quote[0] && in_quotes)
                {
                        in_quotes = 0;
                        sub_string[strlen(sub_string) - 1] = '\0';
                }

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
}

int main(void)
{
        const char *prompt = "#cisfun$ ";
        char *input;
        char **sub_strings;
        char *string;
        /**int max_processes = 10;*/
        int status;
        int i = 1;
        /*int num_processes = 0;*/
        pid_t pid;

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

                if (sub_strings != NULL && sub_strings[0] != NULL)
                {
                        if (strcmp(sub_strings[0], "exit") == 0)
                        {
                                write(STDOUT_FILENO, "Exiting the shell.\n", strlen("Exiting the shell.\n"));
                                break;
                        }
                        else if (strcmp(sub_strings[0], "echo") == 0)
                        {
                                while (sub_strings[i] != NULL)
                                {
                                        if (sub_strings[i][0] == '"' && sub_strings[i][strlen(sub_strings[i]) - 1] == '"')
                                        {
                                                string = sub_strings[i];
                                                string[strlen(sub_strings[i]) - 1] = '\0';
                                                write(STDOUT_FILENO, string + 1, strlen(string + 1));
                                        }
                                        else
                                        {
                                                write(STDOUT_FILENO, sub_strings[i], strlen(sub_strings[i]));
                                        }
                                        write(STDOUT_FILENO, " ", 1);
                                        i++;
                                }
                                write(STDOUT_FILENO, "\n", 1);
                        }
                        else
                        {
                                pid = fork();
                                if (pid == -1)
                                {
                                        continue;
                                }
                                else if (pid == 0)
                                {
                                        execvp(sub_strings[0], sub_strings);
                                        write(STDERR_FILENO, "Command not executed. Enter a valid command\n",
                                                strlen("Command not executed. Enter a valid command\n"));
                                        exit(1);
                                }
                                else
                                {
                                        waitpid(pid, &status, 0);
                                }
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
