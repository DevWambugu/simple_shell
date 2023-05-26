#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
*main - runs a simple shell
*@ac: argument count
*@argv: argument vector
*
*Return: 0 or 1
*/
int main(int ac, char **argv)
{
    char *prompt = "cisfun$ ";
    char *lineptr = NULL, *copy_line_pointer = NULL;
    size_t n = 0;
    ssize_t no_of_characters_read;
    const char *delimeters = " \n";
    int num_substrings = 0;
    char *substring;
    int i;

    (void)ac;

    while (1)
    {
        write(STDOUT_FILENO, prompt, strlen(prompt));

        no_of_characters_read = getline(&lineptr, &n, stdin);

        if (no_of_characters_read == -1)
        {
            printf("Exiting shell....\n");
            return (-1);
        }

        copy_line_pointer = malloc(sizeof(char) * no_of_characters_read);
        if (copy_line_pointer == NULL)
        {
            perror("tsh: memory allocation error");
            return (-1);
        }

        strcpy(copy_line_pointer, lineptr);

        substring = strtok(lineptr, delimeters);

        while (substring != NULL)
        {
            num_substrings++;
            substring = strtok(NULL, delimeters);
        }
        num_substrings++;

        argv = malloc(sizeof(char *) * num_substrings);

        substring = strtok(copy_line_pointer, delimeters);

        for (i = 0; substring != NULL; i++)
        {
            argv[i] = malloc(sizeof(char) * strlen(substring));
            strcpy(argv[i], substring);

            substring = strtok(NULL, delimeters);
        }
        argv[i] = NULL;

        execmd(argv);
    }

    free(copy_line_pointer);
    free(lineptr);

    return (0);
}
