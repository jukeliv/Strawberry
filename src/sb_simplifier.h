
#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include "extern/utils.h"
#include "sb_token.h"

typedef enum
{
    SI_VAR,
    SI_SET,
    SI_MATH,
    SI_PRINT
} SI_Type;

typedef struct
{
    unsigned long heap;
    unsigned long count; 
    String* content;
} String_List;

typedef struct
{
    SI_Type type;
    String_List arguments;
} SI_Token;

typedef struct
{
    unsigned long heap;
    unsigned long count; 
    SI_Token* content;
} SI_List;

String_List String_List_init(String_List* list)
{
    list->count = 0;
    list->heap = 1;
    list->content = (String*)malloc(sizeof(String));
}

void String_List_Push(String_List* list, String str)
{
    while(list->count >= list->heap)
    {
        list->heap *= 2;
        list->content = (String*)realloc(list->content, sizeof(String)*list->heap);
    }
    list->content[list->count++] = str;
}

SI_Token SI_Token_new(SI_Type type, String_List list)
{
    SI_Token token;
    token.type = type;
    token.arguments = list;
    return token;
}

void SI_Push(SI_List* list, SI_Token tok)
{
    while(list->count >= list->heap)
    {
        list->heap *= 2;
        list->content = (SI_Token*)realloc(list->content, sizeof(SI_Token)*list->heap);
    }
    list->content[list->count++] = tok;
}

SI_List generate_list(Token_List list)
{
    SI_List ret;
    for(unsigned int i = 0; i < list.count; ++i)
    {
        if(list.content[i].type == TOK_ID)
        {
            int op = -1;
            if(list.content[i].type == TOK_SET)
                op = 0;
            else if(list.content[i].type == TOK_ADDITION)
                op = 1;
            else if(list.content[i].type == TOK_SUBTRACTION)
                op = 2;
            
            ++i;

            if(op == -1)
            {
                printf("ERROR: not valid expression SIMPLIFIER\n");
            }
            if(op != 0)
            {
                SI_Push(&ret, SI_Token_new(SI_MATH, {"operation", "operation", "second, var"}, 4));
            }
        }
        else if(list.content[i].type == TOK_INT)
        {
            if(var_index >= heap)
            {
                heap *= 2;
                variables = (variable*)realloc(variables, sizeof(variable)*heap);
            }
            variables[var_index].id = list.content[++i].value.content;
            ++i;
            variables[var_index].value = String_copy(list.content[++i].value);
            variables[var_index].type = INT;
            ++var_index;
        }
        else if(list.content[i].type == TOK_STR)
        {
            if(strcmp(list.content[i].value.content, "string"))
                return 1;
            if(var_index >= heap)
            {
                heap *= 2;
                variables = (variable*)realloc(variables, sizeof(variable)*heap);
            }
            variables[var_index].id = list.content[++i].value.content;
            ++i;
            variables[var_index].value = String_copy(list.content[++i].value);
            variables[var_index].type = STRING;
            ++var_index;
        }
        else if(list.content[i].type == TOK_STR)
        {
            variables[var_index++].id = list.content[++i].value.content;
            ++i;
            variables[var_index].value = list.content[++i].value;
            variables[var_index].type = INT;
        }
        else if(list.content[i].type == TOK_IF)
        {
            ++i;
            if(list.content[i].type != TOK_EQUALS)
            {
                if(find_var(variables, heap, list.content[i-1].value.content)->type != find_var(variables, heap, list.content[i+1].value.content)->type)
                    return 1;
            }
        }
        else if(list.content[i].type == TOK_PRINT)
        {
            //print "hello world"
            ++i;
            if(list.content[i].type == TOK_NUM)
            {
                printf("%d\n", stoi(list.content[i].value.content));
            }
            else if(list.content[i].type == TOK_STR)
            {
                printf("%s\n", list.content[i].value.content);
            }
            else if(list.content[i].type == TOK_ID)
            {
                variable* var = find_var(variables, heap, list.content[i].value.content);
                if(var == NULL)
                    return 1;
                if(var->type == INT)
                    printf("%d\n", stoi(var->value.content));
                else if(var->type == STRING)
                    printf("%s\n", var->value.content);
            }
        }
    }
}

#endif