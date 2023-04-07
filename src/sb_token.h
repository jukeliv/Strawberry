
#ifndef SB_TOKEN_H_
#define SB_TOKEN_H_

#include "extern/utils.h"

typedef enum
{
    TOK_INT,
    TOK_PRINT,
    TOK_IF,
    TOK_ELSE,
    TOK_END,
    TOK_STR,
    TOK_NUM,
    TOK_ID,
    TOK_OPEN_BRACKET,
    TOK_CLOSE_BRACKET,
    TOK_STOP,
    TOK_SET, // =
    TOK_EQUALS, // == and "is"
    TOK_NOT,    // != and "not"
    TOK_PLUS, // +
    TOK_ADDITION, // +=
    TOK_MINUS, // -
    TOK_SUBTRACTION, // -= 
    TOK_DIVISION, // /
    TOK_DIVIDED, // /=
    TOK_MULTIPLY, // *
    TOK_MULTIPLIED, // *=
    TOK_GRATER, // >
    TOK_LESS, // <
} Token_Type;

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

int Tokenize(const char* source, Token_List* token_list)
{
    char lex[256];
    unsigned int lex_i = 0;

    for(unsigned int i = 0; i < strlen(source); ++i)
    {
        while(source[i] == '\n' || source[i] == ' ' || source[i] == '\r')
            ++i;

        lex_i=0;
        memset(lex, '\0', 256);

        if(source[i] == ';')
        {
            while(source[i] != '\n')
                ++i;
            continue;
        }
        else if(source[i] == '>')
        {
            lex[lex_i++] = '>';
            Token_List_Push(token_list, Token_Init(TOK_GRATER, String_Init_Str(lex)));
            continue;
        }
        else if(source[i] == '<')
        {
            lex[lex_i++] = '<';
            Token_List_Push(token_list, Token_Init(TOK_LESS, String_Init_Str(lex)));
        }
        else if(source[i] == '=')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '=';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_EQUALS, String_Init_Str(lex)));
            }
            else
            {
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_SET, String_Init_Str(lex)));
            }
        }
        else if(source[i] == '!')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '!';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_NOT, String_Init_Str(lex)));
            }
            else
            {
                printf("ERROR: Operator has not been implemented\n");
                return 1;
            }
        }
        else if(source[i] == '+')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '+';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_ADDITION, String_Init_Str(lex)));
            }
            else
            {
                lex[lex_i++] = '+';
                Token_List_Push(token_list, Token_Init(TOK_PLUS, String_Init_Str(lex)));
            }
        }
        else if(source[i] == '-')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '-';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_SUBTRACTION, String_Init_Str(lex)));
            }
            else
            {
                lex[lex_i++] = '-';
                Token_List_Push(token_list, Token_Init(TOK_MINUS, String_Init_Str(lex)));
            }
        }
        else if(source[i] == '*')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '*';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_MULTIPLIED, String_Init_Str(lex)));
            }
            else
            {
                lex[lex_i++] = '*';
                Token_List_Push(token_list, Token_Init(TOK_MULTIPLY, String_Init_Str(lex)));
            }
        }
        else if(source[i] == '/')
        {
            if(source[++i] == '='){
                lex[lex_i++] = '/';
                lex[lex_i++] = '=';
                Token_List_Push(token_list, Token_Init(TOK_DIVIDED, String_Init_Str(lex)));
            }
            else
            {
                lex[lex_i++] = '/';
                Token_List_Push(token_list, Token_Init(TOK_DIVISION, String_Init_Str(lex)));
            }
        }
        else if(source[i] == '(')
        {
            lex[lex_i++] = '(';
            Token_List_Push(token_list, Token_Init(TOK_OPEN_BRACKET, String_Init_Str(lex)));
            continue;
        }
        else if(source[i] == ')')
        {
            lex[lex_i++] = ')';
            Token_List_Push(token_list, Token_Init(TOK_CLOSE_BRACKET, String_Init_Str(lex)));
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
        }
        else if(isnum(source[i]))
        {
            lex[lex_i++] = source[i++];
            while(isnum(source[i]))
            {
                lex[lex_i++] = source[i++];
            }
            Token_List_Push(token_list, Token_Init(TOK_NUM, String_Init_Str(lex)));
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
            else if(!strcmp(lex, "else")){
                Token_List_Push(token_list, Token_Init(TOK_ELSE, String_Init_Str(lex)));
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
            else if(!strcmp(lex, "stop")){
                Token_List_Push(token_list, Token_Init(TOK_STOP, String_Init_Str(lex)));
            }
            else if(!strcmp(lex, "is")){
                Token_List_Push(token_list, Token_Init(TOK_EQUALS, String_Init_Str(lex)));
            }
            else if(!strcmp(lex, "not")){
                Token_List_Push(token_list, Token_Init(TOK_NOT, String_Init_Str(lex)));
            }
            else{
                Token_List_Push(token_list, Token_Init(TOK_ID, String_Init_Str(lex)));
            }
        }
        else
        {
            if(source[i] == '\0')
                break;
            printf("Unknown character (%c : %d)\n", source[i], source[i]);
            return 1;
        }
    }
    return 0;
}
#endif // SB_TOKEN_H_