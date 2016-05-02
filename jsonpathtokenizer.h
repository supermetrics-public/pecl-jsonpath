#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    DEFAULT,
    ROOT,
    WILD_CARD,
    DEEP_SCAN,
    CHILD_KEY,
    TYPE_OPERAND,
    TYPE_OPERATOR,
    TYPE_PAREN,
} token_type;

typedef enum {
    RANGE,
    INDEX,
    ANY,
    SINGLE_KEY,
    FILTER
} child_type;

typedef enum {
    CUR_NODE,
    JSON_PATH,
    CONST_VAL
} operand_type;


typedef enum {
    NODE_VAL,
    STR_VAL,
    NUM_VAL
} expr_type;

typedef enum {
    EQ,
    NE,
    LT,
    LTE,
    GT,
    GTE,
    ISSET,
    OR,
    AND,
    PAREN_LEFT,
    PAREN_RIGHT,
    LITERAL,
    BOOL,
    NODE_NAME
} token;

typedef struct {
    token type;
    char value[100];
    bool value_bool;
    char label[100];
} expr;

struct token {
    token_type type;
    struct {
        char val[100];
        child_type type;
        int index_count;
        int indexes[100];
        expr expr_list[100];
        int expr_count;
    } prop;
};

bool tokenize(char ** input, struct token * tok);

/** START All things imported from expr.h **/

typedef bool (*compare_cb)(expr *, expr *);

void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int * out_count);
bool evaluate_postfix_expression(expr * expr, int count);
compare_cb exec_cb_by_token(token);
token_type get_token_type(token token);

bool compare_lt(expr * lh, expr * rh);
bool compare_gt(expr * lh, expr * rh);
bool compare_and(expr * lh, expr * rh);
bool compare_or(expr * lh, expr * rh);
bool compare_eq(expr * lh, expr * rh);

#define STACK_MAX 100

struct Stack {
    expr     * data[STACK_MAX];
    int     size;
};
typedef struct Stack Stack;

void Stack_Init(Stack *S);
expr * Stack_Top(Stack *S);
void Stack_Push(Stack *S, expr * expr);
void Stack_Pop(Stack *S);

/** END All things imported from expr.h **/

#endif /* TOKENIZER_H */