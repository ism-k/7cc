#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values represent type of token
enum
{
    TK_NUM = 256, // int
    TK_EOF,       // End of Input
};

// Type
typedef struct
{
    int  ty;     // Type of token
    int  val;    // Value
    char *input; // String of input (for error message)
} Token;

Token tokens[100];

// Divide p-strings by each tokens
void tokenize(char *p)
{
    int i = 0;

    while (*p)
    {
        // Skip white space
        if (isspace(*p)) 
        {
            p++;
            continue;
        }
        
        // Operator
        if (*p == '+' || *p == '-') 
        {
            tokens[i].ty    = *p;
            tokens[i].input =  p;
            i++;
            p++;
            continue;
        }

        // Integer
        if (isdigit(*p)) 
        {
            tokens[i].ty    = TK_NUM;
            tokens[i].input = p;
            tokens[i].val   = strtol(p, &p, 10);
            i++;
            continue;
        }
        
        fprintf(stderr, "Unable to tokenize: %s\n", p);
        exit(1);
    }

    tokens[i].ty    = TK_EOF;
    tokens[i].input = p;
}

// Report Error
void error(int i)
{
    fprintf(stderr, "Unexpected Token: %s\n", tokens[i].input);
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // tokens[0] must be int
    if (tokens[0].ty != TK_NUM) 
        error(0);
    
    printf("    mov rax, %d\n", tokens[0].val);

    int i = 1;
    while (tokens[i].ty != TK_EOF)
    {
        if (tokens[i].ty == '+') 
        {
            i++;
            if (tokens[i].ty != TK_NUM) 
                error(i);
            
            printf("    add rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        if (tokens[i].ty == '-') 
        {
            i++;
            if (tokens[i].ty != TK_NUM) 
                error(i);

            printf("    sub rax, %d\n", tokens[i].val);
            i++;
            continue;
        }

        error(i);
    }

    printf("    ret\n");
    return 0;
}