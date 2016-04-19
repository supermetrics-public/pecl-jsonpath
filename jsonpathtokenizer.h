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
    CHILD_KEY
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
    EQ,
    NE,
    LT,
    LTE,
    GT,
    GTE,
    ISSET
} operator_type;

typedef enum {
    NODE_VAL,
    STR_VAL,
    NUM_VAL
} expr_type;

struct token {
    token_type type;
    struct {
        char val[100];
        child_type type;
        int index_count;
        int indexes[100];
        struct {
            expr_type lh_type;
            char lh_val[100];
            expr_type rh_type;
            char rh_val[100];
            operator_type op;
        } expr;
    } prop;
};

bool tokenize(char ** input, struct token * tok);

#endif /* TOKENIZER_H */