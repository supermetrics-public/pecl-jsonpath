#ifndef EXPR_H
#define EXPR_H 1

#include <stdbool.h>

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
    BOOL
} token;

typedef enum {
    TYPE_OPERAND,
    TYPE_OPERATOR,
    TYPE_PAREN,
} token_type;

typedef struct {
    token type;
    char value[100];
    bool value_bool;
} expr;

typedef bool (*compare_cb)(expr *, expr *);

void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int * out_count);
bool evaluate_postfix_expression(expr * expr, int count);
compare_cb exec_cb_by_token(token);
//bool compare_lt(expr * lh, expr * rh);
//bool compare_gt(expr * lh, expr * rh);

#endif /* EXPR_H */