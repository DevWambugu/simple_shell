#include "main.h"

#include <stdio.h>
#include <stdlib.h>

ssize_t custom_getline(char **line_pointer, size_t *buff_size, FILE *stream) {
    ssize_t characters_read;
    size_t buffer_size = 0;

    if (line_pointer == NULL || buff_size == NULL || stream == NULL) {
        return -1;
    }

    if (*line_pointer == NULL) {
        *buff_size = 0;
        *line_pointer = malloc(1);
        if (*line_pointer == NULL) {
            return -1;
        }
    }

    characters_read = getline(line_pointer, &buffer_size, stream);

    if (characters_read == -1) {
        free(*line_pointer);
        *line_pointer = NULL;
        *buff_size = 0;
    } else {
        *buff_size = buffer_size;
    }

    return characters_read;
}

