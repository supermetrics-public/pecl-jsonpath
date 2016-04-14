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
     CONST_VAL,
     REGEX
} operand_type;

typedef enum {
    EQ,
    NE,
    LT,
    LTE,
    GT,
    GTE
} operator_type;

struct token {
    token_type type;
    struct {
        char val[100];
        child_type type;
        int index_count;
        int indexes[100];
        struct {
            char rh[100];
            char lh[100];
            operator_type op;
        } expr;
    } prop;
};

bool tokenize(char ** input, struct token * tok);

#endif /* TOKENIZER_H */