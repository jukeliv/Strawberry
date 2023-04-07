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
    if(Tokenize(prog, &token_list) == 1)
    {
        printf("ERROR: While tokenizing file!!!\n");
        return 1;
    }

    var* variables = (var*)malloc(sizeof(var));
    unsigned char skip_else = 1;
    unsigned int heap = 1;
    unsigned int var_index = 0;
    if(token_list.content[token_list.count-2].type != TOK_STOP)
    {
        printf("ERROR: You forgot to add the \"stop\" keyword at the end of your program");
        return 1;
    }
    for(unsigned int i = 0; i < token_list.count; ++i)
    {
        switch(token_list.content[i].type)
        {
            case TOK_IF:
            {
                ++i;
                skip_else = 1; // reseting variable, it will be set to 0 if you find an if
                int op = -1;
                if(token_list.content[++i].type == TOK_EQUALS)
                    op = 0;
                else if(token_list.content[i].type == TOK_GRATER)
                    op = 1;
                else if(token_list.content[i].type == TOK_LESS)
                    op = 2;
                else if(token_list.content[i].type == TOK_NOT)
                    op = 3;
                
                if(op == -1)
                {
                    printf("ERROR: Unknown operator (%s : %d)\n", token_list.content[i].value.content, i);
                    return 1;
                }
                var* a = find_var(variables, heap, token_list.content[i-1].value.content);
                var* b = find_var(variables, heap, token_list.content[i+1].value.content);

                String valA = String_Init();
                String valB = String_Init();

                var_type typeA = INT;
                var_type typeB = INT;

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
                    valA = String_copy(token_list.content[i-1].value);
                    if(token_list.content[i-1].type == TOK_NUM)
                        typeA = INT;
                    else if(token_list.content[i-1].type == TOK_STR)
                        typeA = STRING;
                }
                else
                {
                    valA = String_copy(a->value);
                    typeA = a->type;
                }
                
                if(b == NULL){
                    valB = String_copy(token_list.content[i+1].value);
                    if(token_list.content[i+1].type == TOK_NUM)
                        typeB = INT;
                    else if(token_list.content[i+1].type == TOK_STR)
                        typeB = STRING;
                }
                else{
                    valB = String_copy(b->value);
                    typeB = b->type;
                }

                ++i;

                if(typeA != typeB)
                {
                    printf("ERROR: Can't compare two variables/values with different data types");
                    return 1;
                }

                char check = 1;
                if(typeA == INT)
                {
                    if(op == 0)
                    {
                        if(stoi(valA.content) == stoi(valB.content))
                            check = 0;
                    }
                    else if(op == 1)
                    {
                        if(stoi(valA.content) > stoi(valB.content))
                            check = 0;
                    }
                    else if(op == 2)
                    {
                        if(stoi(valA.content) < stoi(valB.content))
                            check = 0;
                    }
                    else if(op == 3)
                    {
                        if(stoi(valA.content) != stoi(valB.content))
                            check = 0;
                    }
                }
                else if(typeA == STRING)
                {
                    if(op == 0)
                    {
                        if(!strcmp(valA.content, valB.content))
                            check = 0;
                    }
                    else if(op == 1)
                    {
                        if(valA.count > valB.count)
                            check = 0;
                    }
                    else if(op == 2)
                    {
                        if(valA.count < valB.count)
                            check = 0;
                    }
                    else if(op == 3)
                    {
                        if(strcmp(valA.content, valB.content))
                            check = 0;
                    }
                }
                
                if(check)
                {
                    for(int j = i+1; j < token_list.count; ++j)
                    {
                        if(token_list.content[j].type == TOK_ELSE)
                            skip_else = 0;
                        if(token_list.content[j].type == TOK_END || token_list.content[j].type == TOK_ELSE)
                        {
                            i = j;
                            break;
                        }
                    }
                }
            }
            break;
            case TOK_ELSE:
            {
                if(skip_else == 1)
                {
                    for(int j = i+1; j < token_list.count; ++j)
                    {
                        if(token_list.content[j].type == TOK_END || token_list.content[j].type == TOK_ELSE)
                        {
                            i = j;
                            skip_else = 0;
                            break;
                        }
                    }
                }
            }
            break;
            case TOK_ID:
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
                else if(token_list.content[i].type == TOK_MULTIPLIED)
                    op = 3;
                else if(token_list.content[i].type == TOK_DIVIDED)
                    op = 4;
                
                if(op == -1)
                {
                    printf("ERROR: Unknown operator (%s : %d)\n", token_list.content[i].value.content, i);
                    return 1;
                }

                ++i;

                var* b = find_var(variables, heap, token_list.content[i].value.content);
                if(b == NULL && token_list.content[i].type == TOK_ID)
                {
                    printf("ERROR: Unknown variable in operation (A) %s (B)unknown\n", token_list.content[i-1].value.content);
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
                        else if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(token_list.content[i].value.content);
                            a->value = String_Init_Str(itos(valA + valB));
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
                        else if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(token_list.content[i].value.content);
                            a->value = String_Init_Str(itos(valA - valB));
                        }
                    }else if(op == 3)
                    {
                        if(a->type == INT && token_list.content[i].type != TOK_NUM)
                        {
                            printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                            return 1;
                        }
                        if(a->type == STRING)
                        {
                            printf("ERROR: Can't multiply with strings\n");
                            return 1;
                        }
                        else if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(token_list.content[i].value.content);
                            a->value = String_Init_Str(itos(valA * valB));
                        }
                    }
                    else if(op == 4)
                    {
                        if(a->type == INT && token_list.content[i].type != TOK_NUM)
                        {
                            printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                            return 1;
                        }
                        if(a->type == STRING)
                        {
                            printf("ERROR: Can't divide with strings\n");
                            return 1;
                        }
                        else if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(token_list.content[i].value.content);
                            a->value = String_Init_Str(itos(valA / valB));
                        }
                    }
                }
                else
                {
                    if(op == 0)
                    {
                        if(a->type != b->type)
                        {
                            printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                            return 1;
                        }
                        a->value = b->value;
                    }
                    else if(op == 1)
                    {
                        if(a->type != b->type)
                        {
                            printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                            return 1;
                        }
                        if(a->type == STRING)
                            String_Concat(&a->value, b->value.content);
                        else if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(b->value.content);
                            a->value = String_Init_Str(itos(valA + valB));
                        }
                    }
                    else if(op == 2)
                    {
                        if(a->type != b->type)
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
                            int valB = stoi(b->value.content);
                            a->value.content = (char*)itos(valA - valB);
                        }
                    }
                    else if(op == 3)
                    {
                        if(a->type != b->type)
                        {
                            printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                            return 1;
                        }
                        if(a->type == STRING)
                        {
                            printf("ERROR: Can't multiply with strings\n");
                            return 1;
                        }
                        if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(b->value.content);
                            a->value.content = (char*)itos(valA * valB);
                        }
                    }
                    else if(op == 4)
                    {
                        if(a->type != b->type)
                        {
                            printf("ERROR: Conflicting types, (A)type == INT, (B) != Numeric");
                            return 1;
                        }
                        if(a->type == STRING)
                        {
                            printf("ERROR: Can't divide with strings\n");
                            return 1;
                        }
                        if(a->type == INT)
                        {
                            int valA = stoi(a->value.content);
                            int valB = stoi(b->value.content);
                            a->value.content = (char*)itos(valA / valB);
                        }
                    }
                }
            }
            break;
            case TOK_INT:
            {
                if(var_index >= heap)
                {
                    heap *= 2;
                    variables = (var*)realloc(variables, sizeof(var)*heap);
                }
                variables[var_index].id = token_list.content[++i].value.content;
                if(token_list.content[++i].type != TOK_SET)
                {
                    printf("ERROR: Unknown variable initialization (%s)?\n", token_list.content[i].value.content);
                    return 1;
                }
                String val = String_Init();
                if(token_list.content[++i].type != TOK_NUM)
                {
                    if(token_list.content[i].type != TOK_ID)
                    {
                        printf("ERROR: Can't set an int to another data type\n");
                        return 1;
                    }
                    else
                    {
                        var* variable = find_var(variables, heap, token_list.content[i].value.content);
                        if(variable == NULL)
                        {
                            printf("ERROR: Can't set a variable to an unknown variable (%s)\n", token_list.content[i].value.content);
                            return 1;
                        }
                        if(variable->type != INT)
                        {
                            printf("ERROR: Can't set an int to another data type\n");
                            return 1;
                        }
                    }
                }
                var* variable = find_var(variables, heap, token_list.content[i].value.content);
                if(variable == NULL)
                    val = String_Init_Str(itos(stoi(token_list.content[i].value.content)));
                else
                    val = String_Init_Str(itos(stoi(variable->value.content)));
                variables[var_index].value = String_copy(val);
                variables[var_index].type = INT;
                ++var_index;
            }
            break;
            case TOK_STR:
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
                String val = String_Init();
                if(token_list.content[++i].type != TOK_STR)
                {
                    if(token_list.content[i].type != TOK_ID)
                    {
                        printf("ERROR: Can't set an string to another data type\n");
                        return 1;
                    }
                    else
                    {
                        var* variable = find_var(variables, heap, token_list.content[i].value.content);
                        if(variable == NULL)
                        {
                            printf("ERROR: Can't set a variable to an unknown variable (%s)\n", token_list.content[i].value.content);
                            return 1;
                        }
                        if(variable->type != STRING)
                        {
                            printf("ERROR: Can't set an string to another data type\n");
                            return 1;
                        }
                    }
                }
                var* variable = find_var(variables, heap, token_list.content[i].value.content);
                if(variable == NULL)
                    val = String_copy(token_list.content[i].value);
                else
                    val = String_copy(variable->value);
                variables[var_index].value = String_copy(val);
                variables[var_index].type = STRING;
                ++var_index;
            }
            break;
            case TOK_PRINT:
            {
                //print "hello world"
                ++i;
                if(token_list.content[i].type != TOK_ID)
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
            break;
            case TOK_STOP:
            {
                //stop 0
                if(token_list.content[++i].type == TOK_NUM)
                {
                    return stoi(token_list.content[++i].value.content);
                }
                else if(token_list.content[i].type == TOK_ID)
                {
                    var* a = find_var(variables, heap, token_list.content[i].value.content);
                    if(a == NULL)
                    {
                        printf("ERROR: Can't stop execution passing a unknown variable (%s)\n", token_list.content[i].value.content);
                        return 1;
                    }
                }
                else
                {
                    printf("ERROR: Can't stop execution passing a unknown type\n");
                    return 1;
                }
            }
            break;
            default:
                continue;
            break;
        }
    }
    return 0;
}

#endif // SB_INTERPRETER_H_