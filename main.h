#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void execmd(char **argv);
char *get_location(char *command);
ssize_t custom_getline(char **line_pointer, size_t *buff_size, FILE *stream);
#endif
