#ifndef PARSER_H
#define PARSER_H 1

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_NODE_DEPTH 5
#define MAX_NODE_NAME_LEN 25	/* Includes null-terminator */

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
    FLTR_RANGE,
    FLTR_INDEX,
    FLTR_WILD_CARD,
    FLTR_NODE,
    FLTR_EXPR
} enum_filter_type;

typedef enum {
    EQ,				//0
    NE,				//1
    LT,				//2
    LTE,			//3
    GT,				//4
    GTE,			//5
    ISSET,			//6
    OR,				//7
    AND,			//8
    PAREN_LEFT,			//9
    PAREN_RIGHT,		//10
    LITERAL,			//11
    BOOL,			//12
    NODE_NAME			//13
} token;

typedef struct {
    token type;
    char value[100];
    bool value_bool;
    char label[MAX_NODE_DEPTH][MAX_NODE_NAME_LEN];
    int label_count;
} expr;

typedef struct {
    token_type type;
    char node_value[100];
    enum_filter_type filter_type;
    int index_count;
    int indexes[100];
    expr expressions[100];
    int expression_count;
} parse_token;

bool tokenize(char **input, parse_token * tok);

typedef bool(*compare_cb) (expr *, expr *);

void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int *out_count);
bool evaluate_postfix_expression(expr * expr, int count);
compare_cb exec_cb_by_token(token);
token_type get_token_type(token token);

bool compare_lt(expr * lh, expr * rh);
bool compare_gt(expr * lh, expr * rh);
bool compare_and(expr * lh, expr * rh);
bool compare_or(expr * lh, expr * rh);
bool compare_eq(expr * lh, expr * rh);
bool isset2(expr * lh, expr * rh);	// lh = rh

#define STACK_MAX 100

struct Stack {
    expr *data[STACK_MAX];
    int size;
};
typedef struct Stack Stack;

void Stack_Init(Stack * S);
expr *Stack_Top(Stack * S);
void Stack_Push(Stack * S, expr * expr);
void Stack_Pop(Stack * S);

void build_parse_tree(lex_token lex_tok[100],
		      char lex_tok_values[100][100], int lex_tok_count, parse_token * tok, int *tok_count);

#endif				/* PARSER_H */
