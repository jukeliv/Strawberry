#include "sb_interpreter.h"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("ERROR: Not enough arguments!!!\n");
        return 1;
    }
    if(!strcmp(argv[1], "-h"))
    {
        printf("Strawberry - Interpreted programming language\n");
        printf("\tStrawberry is a staticly typed programming language for making video games\n\n");
        printf("Commands\n");
        printf("\t-h - this\n");
        printf("\t-c - check for errors in your code before interpreting ( not implemented )\n");
        printf("\t-i - interpret your code\n");
        return 0;
    }
    else if(!strcmp(argv[1], "-i"))
    {
        if(argc < 3)
        {
            printf("ERROR: No file has been provided for the interpreter!!!\n");
            return 1;
        }
        const char* prog = read_file(argv[2]);
        return interpret(prog);
    }
    else if(!strcmp(argv[1], "-c"))
    {
        if(argc < 3)
        {
            printf("ERROR: No file has been provided for error checking!!!\n");
            return 1;
        }
        printf("TODO: add checking for errors in the code\n");
        return 0;
    }
    else
    {
        printf("Unkown input!!!\n");
        return 1;
    }
}