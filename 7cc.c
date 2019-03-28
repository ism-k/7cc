#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// global variable
int pos = 0;

//////////////////////////////////////////
// Tokenizer
//////////////////////////////////////////

// vector
typedef struct
{
    void **data;
    int capacity; // size of buffer
    int len;      // number of elements
} Vector;

Vector *new_vector()
{
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

void vec_push(Vector *vec, void *elem)
{
    if (vec->capacity == vec->len)
    {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}

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
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') 
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
/*void error(int i)
{
    fprintf(stderr, "Unexpected Token: %s\n", tokens[i].input);
    exit(1);
}*/

//////////////////////////////////////////
// Abstract Syntax Tree
//////////////////////////////////////////

enum
{
    ND_NUM = 256, // Type of Int Node
};

typedef struct Node
{
    int ty;           // operator or ND_NUM
    struct Node *lhs; //  left-hand side
    struct Node *rhs; // right-hand side
    int val;          // value when ty is ND_NUM
} Node;

// Create Node (2-term operator)
Node *new_node(int ty, Node *lhs, Node *rhs)
{
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// Create Node (number)
Node *new_node_num(int val)
{
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

// Check whether a next token is an expected type
int consume(int ty)
{
    if (tokens[pos].ty != ty)
        return 0;
    pos++;
    return 1;
}

// parser
Node *add();
Node *mul();
Node *term();

Node *term()
{
    if (consume('('))
    {
        Node *node = add();
        if (!consume(')')) 
            fprintf(stderr, "No mutch parenthesis: %s", tokens[pos].input);
        return node;
    }

    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);
    
    fprintf(stderr, "This token is neither Number nor Parenthesis: %s", tokens[pos].input);
}

Node *mul()
{
    Node *node = term();

    for( ; ; )
    {
        if (consume('*'))
        {
            node = new_node('*', node, term());
        }
        else if (consume('/'))
        {
            node = new_node('/', node, term());
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();

    for( ; ; )
    {
        if (consume('+'))
        {
            node = new_node('+', node, mul());
        }
        else if (consume('-'))
        {
            node = new_node('-', node, mul());
        }
        else
        {
            return node;
        }
    }
}

//////////////////////////////////////////
// Stack Compiler
//////////////////////////////////////////

void gen(Node *node)
{
    if (node->ty == ND_NUM)
    {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->ty)
    {
        case '+':
            printf("    add rax, rdi\n");
            break;
    
        case '-':
            printf("    sub rax, rdi\n");
            break;

        case '*':
            printf("    mul rdi\n");
            break;

        case '/':
            printf("    mov rdx, 0\n");
            printf("    div rdi\n");
    }

    printf("    push rax\n");
}

//////////////////////////////////////////
// Main Function
//////////////////////////////////////////

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    if (strcmp(*argv[1], "-test") == 0)
    {
        runtest();
    }

    // tokenize and parse
    tokenize(argv[1]);
    Node *node = add();

    // header
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Code generation
    gen(node);

    // Pop answer from stuck-top
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}

//////////////////////////////////////////
// Test
//////////////////////////////////////////

int expect(int line, int expected, int actual)
{
    if (expected == actual)
        return;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void runtest()
{
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for(int i = 0; i < 100; i++)
        vec_push(vec, (void *)i);
    
    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (int)vec->data[0]);
    expect(__LINE__, 50, (int)vec->data[50]);
    expect(__LINE__, 99, (int)vec->data[99]);
    
    printf("OK\n");
}