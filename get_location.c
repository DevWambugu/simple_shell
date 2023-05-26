#include "main.h"

char *get_location(char *command){
    char *path, *copy_of_the_path, *path_token, *file_path;
    int command_length, directory_length;
    struct stat buff;
    
    path = getenv("PATH");

    if (path){
        copy_of_the_path = strdup(path);
        command_length = strlen(command);

        path_token = strtok(copy_of_the_path, ":");

        while(path_token != NULL){
            directory_length = strlen(path_token);
            file_path = malloc(command_length + directory_length + 2);
            strcpy(file_path, path_token);
            strcat(file_path, "/");
            strcat(file_path, command);
            strcat(file_path, "\0");

            if (stat(file_path, &buff) == 0){
                free(copy_of_the_path);
                return (file_path);
            }
            else{
                free(file_path);
                path_token = strtok(NULL, ":");
            }
        }

        free(copy_of_the_path);

        if (stat(command, &buff) == 0)
        {
            return (command);
        }

        return (NULL);
    }

    return (NULL);
}

