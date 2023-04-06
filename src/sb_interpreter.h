#ifndef SB_INTERPRETER_H_
#define SB_INTERPRETER_H_

#include "sb_token.h"

typedef enum
{
    INT,
    STRING
}var_type;
typedef struct
{
    const char* id;
    var_type type;
    String value;
} var;

var* find_var(var* vars, unsigned int len, const char* id)
{
    for(int i = 0; i < len; ++i)
    {
        if(!strcmp(vars[i].id, id))
            return &vars[i];
    }
    return NULL;
}

int interpret(const char* prog)
{
    Token_List token_list;
    Token_List_Init(&token_list);
    Tokenize(prog, &token_list);
    
    var* variables = (var*)malloc(sizeof(var));
    unsigned int heap = 1;
    unsigned int var_index = 0;
    
    for(unsigned int i = 0; i < token_list.count; ++i)
    {
        //printf("%d\n", i);
        if(token_list.content[i].type == TOK_ID)
        {
            var* a = find_var(variables, heap, token_list.content[i++].value.content);
            if(a == NULL)
            {
                printf("ERROR: Unknown variable (A)%s\n", token_list.content[i-1].value.content);
                return 1;
            }
            
            int op = -1;
            if(token_list.content[i].type == TOK_SET)
                op = 0;
            else if(token_list.content[i].type == TOK_ADDITION)
                op = 1;
            else if(token_list.content[i].type == TOK_SUBTRACTION)
                op = 2;
            
            ++i;

            if(op == -1)
                return 1;

            var* b = find_var(variables, heap, token_list.content[i].value.content);
            if(b == NULL && token_list.content[i].type == TOK_ID)
            {
                printf("ERROR: Unknown set\n");
                return 1;
            }
                
            if(token_list.content[i].type != TOK_ID)
            {
                if(op == 0)
                {
                    if(a->type == INT && token_list.content[i].type != TOK_NUM)
                    {
                        printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                        return 1;
                    }
                    else if(a->type == STRING && token_list.content[i].type != TOK_STR)
                    {
                        printf("ERROR: Conflicting types, (A)type == STRING, (B) != String");
                        return 1;
                    }
                    a->value = token_list.content[i].value;
                }
                else if(op == 1)
                {
                    if(a->type == INT && token_list.content[i].type != TOK_NUM)
                    {
                        printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                        return 1;
                    }
                    else if(a->type == STRING && token_list.content[i].type != TOK_STR)
                    {
                        printf("ERROR: Conflicting types, (A)type == STRING, (B) != String");
                        return 1;
                    }

                    if(a->type == STRING)
                        String_Concat(&a->value, token_list.content[i].value.content);
                    
                    if(a->type == INT)
                    {
                        int valA = stoi(a->value.content);
                        printf("%d\n", valA);
                        int valB = stoi(token_list.content[i].value.content);
                        printf("%d\n", valB);
                        a->value.content = (char*)itos(valA + valB);
                    }
                }else if(op == 2)
                {
                    if(a->type == INT && token_list.content[i].type != TOK_NUM)
                    {
                        printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                        return 1;
                    }
                    if(a->type == STRING)
                    {
                        printf("ERROR: Can't subtract with strings\n");
                        return 1;
                    }
                    if(a->type == INT)
                    {
                        int valA = stoi(a->value.content);
                        int valB = stoi(token_list.content[i].value.content);
                        const char* val = itos(valA - valB);
                        a->value.count = strlen(val);
                        a->value.heap = strlen(val)-1;
                        a->value.content = (char*)val;
                    }
                }
            }
            else
            {
                if(op == 0)
                {
                    if(a->type != b->type)
                    {
                        printf("ERROR: trying to set variable(A) to variable(B) when (A)type != (B)type\n");
                        return 1;
                    }
                    a->value = b->value;
                }
                if(op == 1)
                {
                    if(a->type != b->type)
                    {
                        printf("ERROR: (A)type conflicting with (B)type in addition\n");
                        return 1;
                    }
                    if(a->type == STRING)
                        String_Concat(&a->value, b->value.content);
                    if(a->type == INT)
                    {
                        int valA = stoi(a->value.content);
                        int valB = stoi(b->value.content);
                        const char* val = itos(valA + valB);
                        a->value.count = strlen(val);
                        a->value.heap = strlen(val)-1;
                        a->value.content = (char*)val;
                    }
                }
                if(op == 2)
                {
                    if(a->type != b->type)
                    {
                        printf("ERROR: (A)type conflicting with (B)type in subtraction\n");
                        return 1;
                    }
                    if(a->type == STRING)
                    {
                        printf("ERROR: Can't subtract with strings\n");
                        return 1;
                    }
                    if(a->type == INT)
                    {
                        int valA = stoi(a->value.content);
                        int valB = stoi(b->value.content);
                        a->value.content = (char*)itos(valA + valB);
                    }
                }
            }
        }
        else if(token_list.content[i].type == TOK_INT)
        {
            if(var_index >= heap)
            {
                heap *= 2;
                variables = (var*)realloc(variables, sizeof(var)*heap);
            }
            variables[var_index].id = token_list.content[++i].value.content;
            ++i;
            if(token_list.content[++i].type != TOK_NUM)
            {
                printf("ERROR: Can't set an int to another data type\n");
                return 1;
            }
            variables[var_index].value = String_copy(String_Init_Str(itos(stoi(token_list.content[i].value.content))));
            variables[var_index].type = INT;
            ++var_index;
        }
        else if(token_list.content[i].type == TOK_STR)
        {
            if(strcmp(token_list.content[i].value.content, "string"))
            {
                printf("ERROR: Unknown use of \"string\"\n");
                return 1;
            }
            if(var_index >= heap)
            {
                heap *= 2;
                variables = (var*)realloc(variables, sizeof(var)*heap);
            }
            variables[var_index].id = token_list.content[++i].value.content;
            ++i;
            if(token_list.content[++i].type != TOK_STR)
            {
                printf("ERROR: Can't set an string to another data type\n");
                return 1;
            }
            variables[var_index].value = String_copy(token_list.content[i].value);
            variables[var_index].type = STRING;
            ++var_index;
        }
        else if(token_list.content[i].type == TOK_STR)
        {
            variables[var_index++].id = token_list.content[++i].value.content;
            ++i;
            variables[var_index].value = token_list.content[++i].value;
            variables[var_index].type = INT;
        }
        else if(token_list.content[i].type == TOK_IF)
        {
            ++i;
            if(token_list.content[++i].type != TOK_EQUALS)
            {
                printf("ERROR: Check statement has not been implemented\n");
                return 1;
            }
            var* a = find_var(variables, heap, token_list.content[i-1].value.content);
            var* b = find_var(variables, heap, token_list.content[i+1].value.content);
            String valA = String_Init();
            String valB = String_Init();
            var_type typeA = 0;
            var_type typeB = 0;

            if(a == NULL && token_list.content[i-1].type == TOK_ID)
            {
                printf("ERROR: Unknown variable in if statement (A)unknown == (B)");
                return 1;
            }
            if(b == NULL && token_list.content[i+1].type == TOK_ID)
            {
                printf("ERROR: Unknown variable in if statement (A) == (B)unknown");
                return 1;
            }

            if(a == NULL){
                valA = token_list.content[i-1].value;
                if(token_list.content[i-1].type == TOK_NUM)
                    typeA = INT;
                else if(token_list.content[i-1].type == TOK_STR)
                    typeA = STRING;
            }
            else
            {
                valA = a->value;
                typeA = a->type;
            }
            
            if(b == NULL){
                valB = token_list.content[i+1].value;
                if(token_list.content[i+1].type == TOK_NUM)
                    typeB = INT;
                else if(token_list.content[i+1].type == TOK_STR)
                    typeB = STRING;
            }
            else{
                valB = b->value;
                typeB = b->type;
            }
            
            if(typeA != typeB)
            {
                printf("ERROR: Can't compare two variables/values with different data types");
                return 1;
            }
            char check = 1;
            if(typeA == INT)
            {
                if(stoi(valA.content) == stoi(valB.content))
                    check = 0;
            }
            else if(typeA == STRING)
            {
                if(!strcmp(valA.content, valB.content))
                    check = 0;
            }
            
            if(check)
            {
                for(int j = i; j < token_list.count; ++j)
                {
                    if(token_list.content[j].type == TOK_END)
                    {
                        i = j;
                        break;
                    }
                }
            }
            else
            {
                ++i;
            }
        }
        else if(token_list.content[i].type == TOK_PRINT)
        {
            //print "hello world"
            if(token_list.content[++i].type != TOK_ID)
            {
                printf("%s\n", token_list.content[i].value.content);
            }
            else
            {
                var* variable = find_var(variables, heap, token_list.content[i].value.content);
                if(variable == NULL)
                {
                    printf("ERROR: trying to print unknown variable (%s)\n", token_list.content[i].value.content);
                    return 1;
                }
                printf("%s\n", variable->value.content);
            }
        }
    }
    return 0;
}

#endif // SB_INTERPRETER_H_