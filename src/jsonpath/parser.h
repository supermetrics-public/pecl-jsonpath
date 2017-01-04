#ifndef PARSER_H
#define PARSER_H 1

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_NODE_DEPTH 5
#define PARSE_BUF_LEN 100

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
    char value[PARSE_BUF_LEN];
    bool value_bool;
    char * label[MAX_NODE_DEPTH];
    int label_count;
} expr_operator;

typedef struct {
    operator_type type;
    char * node_value;
    int node_value_len;
    filter_type filter_type;
    int index_count;
    int indexes[PARSE_BUF_LEN];
    expr_operator expressions[PARSE_BUF_LEN];
    int expression_count;
} operator;

typedef struct {
    char msg[PARSE_BUF_LEN];
} parse_error;

bool tokenize(char **input, operator * tok);

typedef bool(*compare_cb) (expr_operator *, expr_operator *);

void convert_to_postfix(expr_operator * expr_in, int in_count, expr_operator * expr_out, int *out_count);
bool evaluate_postfix_expression(expr_operator * expr, int count);
compare_cb exec_cb_by_token(expr_op_type);
operator_type get_token_type(expr_op_type);

bool compare_lt(expr_operator * lh, expr_operator * rh);
bool compare_lte(expr_operator * lh, expr_operator * rh);
bool compare_gt(expr_operator * lh, expr_operator * rh);
bool compare_gte(expr_operator * lh, expr_operator * rh);
bool compare_and(expr_operator * lh, expr_operator * rh);
bool compare_or(expr_operator * lh, expr_operator * rh);
bool compare_eq(expr_operator * lh, expr_operator * rh);
bool compare_neq(expr_operator * lh, expr_operator * rh);
bool compare_isset(expr_operator * lh, expr_operator * rh);	// lh = rh

bool build_parse_tree(
    lex_token lex_tok[PARSE_BUF_LEN],
    char lex_tok_values[][PARSE_BUF_LEN],
    int lex_tok_count, 
    operator * tok,
    int *tok_count,
    parse_error * err
);

#endif				/* PARSER_H */
