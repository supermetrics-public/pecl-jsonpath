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
} operator_type;

typedef enum {
    FLTR_RANGE,
    FLTR_INDEX,
    FLTR_WILD_CARD,
    FLTR_NODE,
    FLTR_EXPR
} filter_type;

typedef enum {
    EXPR_EQ,			//0
    EXPR_NE,			//1
    EXPR_LT,			//2
    EXPR_LTE,			//3
    EXPR_GT,			//4
    EXPR_GTE,			//5
    EXPR_ISSET,			//6
    EXPR_OR,			//7
    EXPR_AND,			//8
    EXPR_PAREN_LEFT,		//9
    EXPR_PAREN_RIGHT,		//10
    EXPR_LITERAL,		//11
    EXPR_BOOL,			//12
    EXPR_NODE_NAME		//13
} expr_op_type;

typedef struct {
    expr_op_type type;
    char value[100];
    bool value_bool;
    char label[MAX_NODE_DEPTH][MAX_NODE_NAME_LEN];
    int label_count;
} expr_operator;

typedef struct {
    operator_type type;
    char node_value[100];
    filter_type filter_type;
    int index_count;
    int indexes[100];
    expr_operator expressions[100];
    int expression_count;
} operator;

bool tokenize(char **input, operator * tok);

typedef bool(*compare_cb) (expr_operator *, expr_operator *);

void convert_to_postfix(expr_operator * expr_in, int in_count, expr_operator * expr_out, int *out_count);
bool evaluate_postfix_expression(expr_operator * expr, int count);
compare_cb exec_cb_by_token(expr_op_type);
operator_type get_token_type(expr_op_type);

bool compare_lt(expr_operator * lh, expr_operator * rh);
bool compare_gt(expr_operator * lh, expr_operator * rh);
bool compare_and(expr_operator * lh, expr_operator * rh);
bool compare_or(expr_operator * lh, expr_operator * rh);
bool compare_eq(expr_operator * lh, expr_operator * rh);
bool isset2(expr_operator * lh, expr_operator * rh);	// lh = rh

#define STACK_MAX 100

struct Stack {
    expr_operator *data[STACK_MAX];
    int size;
};
typedef struct Stack Stack;

void Stack_Init(Stack * S);
expr_operator *Stack_Top(Stack * S);
void Stack_Push(Stack * S, expr_operator * expr);
void Stack_Pop(Stack * S);

void build_parse_tree(lex_token lex_tok[100],
		      char lex_tok_values[100][100], int lex_tok_count, operator * tok, int *tok_count);

#endif				/* PARSER_H */
