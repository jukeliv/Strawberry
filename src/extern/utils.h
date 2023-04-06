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

const char* itos(int num)
{
    int size = snprintf(NULL, 0, "%d", num);
    char* str = (char*)malloc(sizeof(char)*size + 1);
    snprintf(str, size + 1, "%d", num);
    return str;
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

    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) {
        printf("Error: Failed to read file\n");
        fclose(file);
        free(buffer);
        return NULL;
    }

    buffer[read_size] = '\0';

    fclose(file);
    return buffer;
}

typedef struct
{
    unsigned long heap;
    unsigned long count; 
    char* content;
} String;

String String_Init()
{
    String str;
    str.count = 0;
    str.heap = 1;
    str.content = (char*)malloc(sizeof(char));
    return str;
}

void String_Push(String* str, char c)
{
    while(str->count+1 >= str->heap)
    {
        str->heap *= 2;
        str->content = (char*)realloc(str->content, sizeof(char)*str->heap);
    }
    str->content[str->count++] = c;
    str->content[str->count] = '\0';
}

String String_Init_Str(const char* src)
{
    String str;
    str.count = strlen(src);
    str.heap = strlen(src)-1;
    str.content = (char*)malloc(sizeof(char)*strlen(src));
    for(int i = 0; i < strlen(src); ++i)
    {
        str.content[i] = src[i];
    }
    str.content[strlen(src)] = '\0';
    return str;
}

String String_copy(String str)
{
    return String_Init_Str(str.content);
}

void String_Concat(String* dst, const char* src)
{
    for(int i = 0; i < strlen(src); ++i)
    {
        String_Push(dst, src[i]);
    }
}

#endif // UTILS_H_