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

// Token tokens[100];

// Variable-length Vector
Vector *tokens;

// a token used temporary
Token *token_tmp;

// Divide p-strings by each tokens
void tokenize(char *p)
{
    int i = 0;
    tokens = new_vector();

    while (*p)
    {
        token_tmp = (Token *)malloc(sizeof(Token *));

        // Skip white space
        if (isspace(*p)) 
        {
            p++;
            continue;
        }
        
        // Operator
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') 
        {
            token_tmp->ty    = *p;
            token_tmp->input =  p;
            vec_push(tokens, (void *)token_tmp);

            //printf("operator: %d %s\n", token_tmp->ty, token_tmp->input);

            i++;
            p++;
            continue;
        }

        // Integer
        if (isdigit(*p)) 
        {
            token_tmp->ty    = TK_NUM;
            token_tmp->input = p;
            token_tmp->val   = strtol(p, &p, 10);
            vec_push(tokens, (void *)token_tmp);

            //printf("integer: %d %s %d\n", token_tmp->ty, token_tmp->input, token_tmp->val);

            i++;
            continue;
        }
        
        fprintf(stderr, "Unable to tokenize: %s\n", p);
        exit(1);
    }
    token_tmp = (Token *)malloc(sizeof(Token *));

    token_tmp->ty    = TK_EOF;
    token_tmp->input = p;
    vec_push(tokens, (void *)token_tmp);
    //printf("EOF: %d %s\n", token_tmp->ty, token_tmp->input);
    //printf("length: %d\n", tokens->len);
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

Token *token_pop;
// Check whether a next token is an expected type
int consume(int ty)
{
    //printf("compare type: %d\n", ty);
    token_pop = (Token *)malloc(sizeof(Token *));
    token_pop = (Token *)tokens->data[pos];
    //printf("pos %d: (ty, input) = (%d, %s)\n", pos, token_pop->ty, token_pop->input);

    if (token_pop->ty != ty)
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
    //printf("              term()\n");

    if (consume('('))
    {
        Node *node = add();
        if (!consume(')'))
        {
            token_pop = (Token *)tokens->data[pos];
            fprintf(stderr, "No mutch parenthesis: %s", token_pop->input);
        }
        return node;
    }

    //printf("pos: %d\n", pos);
    token_pop = (Token *)tokens->data[pos];
    //printf("type: %d\n", token_pop->ty);
    if (token_pop->ty == TK_NUM)
    {
        pos++;
        return new_node_num(token_pop->val);
    }
    
    fprintf(stderr, "This token is neither Number nor Parenthesis: %s", token_pop->input);
}

Node *mul()
{
    //printf("              mul()\n");
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
    //printf("              add()\n");
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
    //printf("Test: Create Vector-unit\n");

    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for(int i = 0; i < 100; i++)
        vec_push(vec, (void *)i);
    
    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (int)vec->data[0]);
    expect(__LINE__, 50, (int)vec->data[50]);
    expect(__LINE__, 99, (int)vec->data[99]);

    //printf("Done\n");

    //printf("Test: Re-cast token\n");
    Vector *vec_token = new_vector();
    //printf("Vector vec_token created\n");

    char *p = "0";
    //printf("char p created\n");
    Token *t;
    t = (Token *)malloc(sizeof(Token *));
    t->ty    = TK_NUM; //printf("State: t->ty = p; done\n");
    t->input = p; //printf("State: t->input = p; done\n");
    t->val   = strtol(p, &p, 10); //printf("State: t->val = strtol(p, &p, 10); done\n");

    vec_push(vec_token, (void *)t);
    //printf("push token done\n");

    Token *t_pop;
    t_pop = (Token *)vec_token->data[0];
    //printf("t_pop->input: %s\n", t_pop->input);
    //printf("t_pop->ty: %d\n", t_pop->ty);
    //printf("t_pop->val: %d\n", t_pop->val);

    //printf("Done\n");

    //printf("OK\n");
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

    if (!strcmp(argv[1], "-test"))
    {
        printf("run test\n");
        runtest();
        return 0;
    }

    // tokenize and parse
    //printf("Start: Tokenizer\n");
    tokenize(argv[1]);
    //printf("Done: Tokenizer\n");
    //printf("Start: Parser\n");
    Node *node = add();
    //printf("Done: Parser\n");

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