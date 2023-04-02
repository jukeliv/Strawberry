#include "sb_interpreter.h"

int main(int argc, char** argv)
{
    if(argc < 1)
    {
        printf("Not enough arguments!!!\n");
        return 1;
    }
    if(!strcmp(argv[1], "-h"))
    {
        printf("Strawberry - Interpreted programming language\n");
        printf("\tStrawberry is a staticly typed programming language for making video games");
        return 0;
    }
    if(!strcmp(argv[1], "-c") && argc > 2)
    {
        const char* prog = read_file(argv[2]);
        interpret(prog);
        return 0;
    }

    printf("Unkown input!!!\n");
    return 1;
}