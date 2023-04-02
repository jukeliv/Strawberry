#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static inline char isnum(char c)
{
    return (c > 47 && c < 58);
}

static inline char ischar(char c)
{
    return ((c > 64 && c < 91) || (c > 96 && c < 123));
}

int stoi(const char* str)
{
    int si = 0;
    for(unsigned long i = 0; i < strlen(str); i++)
    {
        if(!isnum(str[i]))
            return -1;
        si = si * 10 + (str[i] - '0');
    }
    return si;
}

const char* read_file(const char* file_path)
{
    FILE* file = fopen(file_path, "rb");

    if (file == NULL) {
        printf("Error: Failed to open file\n");
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        printf("Error: Failed to allocate memory\n");
        fclose(file);
        return NULL;
    }

    // Read the file contents into the buffer
    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) {
        printf("Error: Failed to read file\n");
        fclose(file);
        free(buffer);
        return NULL;
    }

    // Null-terminate the buffer
    buffer[read_size] = '\0';

    fclose(file);
    return buffer;
}

#endif // UTILS_H_