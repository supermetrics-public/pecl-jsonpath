#ifndef EXPR_H
#define EXPR_H 1

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
    LITERAL
} token;

typedef enum {
    TYPE_OPERAND,
    TYPE_OPERATOR,
    TYPE_PAREN,
} token_type;

typedef struct {
    token type;
    char value[100];
} expr;

void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int * out_count);

#endif /* EXPR_H */