#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    SET,
    ADD,
    SUB,
    JMP,
    JMZ,
    CMP,
    PRINT,
    HLT,
} TOKENS;

int mem[500] = {0};

int main(int argc, char** argv)
{
    if(argc > 1 && !strcmp(argv[1], "-h"))
    {
        printf("PMS - PSP Maker Script ( Bytecode Interpreter )\n");
        printf("Registers range from 0 to 499\n");
        printf("'$' is used to pass values and '&' is to pass the value of a register\n");
        printf("the JMP/JMZ commands jumps to a direct place in the program, and it takes into a count arguments\n");
        printf("not just instructions, so check twice when you use them\n\n");
        printf("SET [reg] [$/&] [value]\n");
        printf("ADD [reg] [$/&] [value]\n");
        printf("SUB [reg] [$/&] [value]\n");
        printf("JMP [$/&] [value]\n");
        printf("JMZ [reg] [$/&] [value]\n");
        printf("CMP [reg] [$/&] [value]\n");
        printf("PRINT [$/&] [value]\n");
        printf("HLT\n");

        exit(0);
    }
    /*
        here the hello world example bytecode
        ```pms
        int a = 10
        if(a == 10)
        {
            print("Hello World")
        }
        ```
    */
    const int prog[] =
    {
        SET, 1, '$', 10,
        CMP, 1, '$', 10,
        JMZ, 0, '$', 15,
        JMP, '$', 18,
        PRINT, '$', (int)"Hello World",
        HLT,
    };
    
    for(unsigned long i = 0; i < sizeof(prog); ++i)
    {
        if(prog[i] == SET)
        {
            int reg = prog[++i];
            int type = prog[++i];
            int val = prog[++i];
            if(type == '$')
                mem[reg] = val;
            else if(type == '&')
                mem[reg] = mem[val];
        }
        else if(prog[i] == ADD)
        {
            int regX = prog[++i];
            int typeY = prog[++i];
            int valY = prog[++i];
            int typeZ = prog[++i];
            int valZ = prog[++i];
            int val = 0;
            if(typeY == '$')
                val = valY;
            else if(typeY == '&')
                val = mem[valY];
            
            if(typeZ == '$')
                val += valZ;
            else if(typeZ == '&')
                val += mem[valZ];

            mem[regX] = val;
        }
        else if(prog[i] == SUB)
        {
            printf("SUB\n");
            int regX = prog[++i];
            int typeY = prog[++i];
            int valY = prog[++i];
            int typeZ = prog[++i];
            int valZ = prog[++i];
            int val = 0;
            if(typeY == '$')
                val = valY;
            else if(typeY == '&')
                val = mem[valY];
            
            if(typeZ == '$')
                val -= valZ;
            else if(typeZ == '&')
                val -= mem[valZ];

            mem[regX] = val;
        }
        else if(prog[i] == JMP)
        {
            int type = prog[++i];
            int val = prog[++i];
            if(type == '$')
                i = val-1;
            else if(type == '&')
                i = mem[val]-1;
        }
        else if(prog[i] == JMZ)
        {
            int reg = prog[++i];
            int type = prog[++i];
            int val = prog[++i];

            if(mem[reg] != 0)
                continue;

            if(type == '$')
                i = val-1;
            else if(type == '&')
                i = mem[val]-1;
        }
        else if(prog[i] == CMP)
        {
            int regX = prog[++i];
            int typeY = prog[++i];
            int valY = 0;
            int valX = mem[regX];
            if(typeY == '$')
                valY = prog[++i];
            else if(typeY == '&')
                valY = mem[prog[++i]];
            if(valX == valY)
            {
                mem[0] = 0;
            }
            else if(valX > valY)
            {
                mem[0] = 1;
            }
            else
            {
                mem[0] = -1;
            }
        }
        else if(prog[i] == PRINT)
        {
            int type = prog[++i];
            int val = prog[++i];
            int msg = 0;
            if(type == '$')
                msg = val;
            else if(type == '&')
                msg = mem[val];
            printf("%s", (char*)msg);
        }
        else if(prog[i] == HLT)
        {
            break;
        }
        else
        {
            printf("unknown command!!!\n");
            exit(1);
        }
    }

    return 0;
}