
#ifndef SB_TOKEN_H_
#define SB_TOKEN_H_

#include "utils.h"

typedef enum
{
    TOK_INT,
    TOK_PRINT,
    TOK_IF,
    TOK_END,
    TOK_STR,
    TOK_NUM,
    TOK_ID,
    TOK_OPEN_BRACKET,
    TOK_CLOSE_BRACKET,
    TOK_SET,
    TOK_EQUALS,
    TOK_PLUS,
    TOK_ADDITION,
    TOK_MINUS,
    TOK_SUBTRACTION,
} Token_Type;

typedef struct
{
    unsigned long heap;
    unsigned long count; 
    char* content;
} String;

typedef struct
{
    Token_Type type;
    String value;
} Token;

typedef struct
{
    unsigned long heap;
    unsigned long count;
    Token* content;
} Token_List;

static Token Token_Init(Token_Type type, String value)
{
    Token token;
    token.type = type;
    token.value = value;
    return token;
}
#if 0
static String String_Init()
{
    String str;
    str.count = 0;
    str.heap = 1;
    str.content = (char*)malloc(sizeof(char));
    return str;
}
#endif

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

void Token_List_Init(Token_List* token_list)
{
    token_list->count = 0;
    token_list->heap = 1;
    token_list->content = (Token*)malloc(sizeof(Token));
}

void Token_List_Push(Token_List* token_list, Token token)
{
    while(token_list->count >= token_list->heap)
    {
        token_list->heap *= 2;
        token_list->content = (Token*)realloc(token_list->content, sizeof(Token)*token_list->heap);
    }
    token_list->content[token_list->count++] = token;
}

void Tokenize(const char* source, Token_List* token_list)
{
    unsigned int i = 0;
    char lex[256];
    unsigned int lex_i = 0;

    while(source[i] != '\0')
    {
        while(source[i] == '\n' || source[i] == ' ')
            ++i;

        lex_i=0;
        memset(lex, '\0', 256);

        if(source[i] == ';')
        {
            while(source[i] != '\n')
                ++i;
            continue;
        }
        else if(source[i] == '=')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '=';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_EQUALS, String_Init_Str(lex)));
                ++i;
            }
            else
            {
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_SET, String_Init_Str(lex)));
                ++i;
            }
            continue;
        }
        else if(source[i] == '+')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '+';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_ADDITION, String_Init_Str(lex)));
                ++i;
            }
            else
            {
                lex[lex_i++] = '+';
                Token_List_Push(token_list, Token_Init(TOK_PLUS, String_Init_Str(lex)));
                ++i;
            }
            continue;
        }
        else if(source[i] == '-')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '-';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_SUBTRACTION, String_Init_Str(lex)));
                ++i;
            }
            else
            {
                lex[lex_i++] = '-';
                Token_List_Push(token_list, Token_Init(TOK_MINUS, String_Init_Str(lex)));
                ++i;
            }
            continue;
        }
        else if(source[i] == '(')
        {
            lex[lex_i++] = '(';
            Token_List_Push(token_list, Token_Init(TOK_OPEN_BRACKET, String_Init_Str(lex)));
            ++i;
            continue;
        }
        else if(source[i] == ')')
        {
            lex[lex_i++] = ')';
            Token_List_Push(token_list, Token_Init(TOK_CLOSE_BRACKET, String_Init_Str(lex)));
            ++i;
            continue;
        }
        else if(source[i] == '"')
        {
            ++i;
            while(source[i] != '"')
            {
                lex[lex_i++] = source[i++];
            }
            String str = String_Init_Str(lex);
            Token_List_Push(token_list, Token_Init(TOK_STR, str));
            ++i;
            continue;
        }
        else if(isnum(source[i]))
        {
            lex[lex_i++] = source[i++];
            while(isnum(source[i]))
            {
                lex[lex_i++] = source[i++];
            }
            Token_List_Push(token_list, Token_Init(TOK_NUM, String_Init_Str(lex)));
            ++i;
            continue;
        }
        else if(ischar(source[i]))
        {
            while(ischar(source[i]))
            {
                lex[lex_i++] = source[i++];
            }
            lex[lex_i] = '\0';
            
            if(!strcmp(lex, "if")){
                Token_List_Push(token_list, Token_Init(TOK_IF, String_Init_Str(lex)));
            }
            else if(!strcmp(lex, "end")){
                Token_List_Push(token_list, Token_Init(TOK_END, String_Init_Str(lex)));
            }
            else if(!strcmp(lex, "int")){
                Token_List_Push(token_list, Token_Init(TOK_INT, String_Init_Str(lex)));
            }
            else if(!strcmp(lex, "string")){
                Token_List_Push(token_list, Token_Init(TOK_STR, String_Init_Str(lex)));
            }
            else if(!strcmp(lex, "print")){
                Token_List_Push(token_list, Token_Init(TOK_PRINT, String_Init_Str(lex)));
            }
            else{
                Token_List_Push(token_list, Token_Init(TOK_ID, String_Init_Str(lex)));
            }
            ++i;
            continue;
        }
        ++i;
    }
}
#endif // SB_TOKEN_H_