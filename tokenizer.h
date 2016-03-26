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
    ANY
} child_type;

struct token {
    token_type type;
    union {
        struct {
            char val[100];
            child_type type;
            int index_count;
            int indexes[100];
        } child;
    } prop;
};

bool tokenize(char ** input, struct token * tok);