#ifndef LEXER_H
#define LEXER_H 1

#include <stdio.h>

typedef enum {
    NOT_FOUND,      /* Token not found */
    ROOT,           /* $ */
    CUR_NODE,       /* @ */
    WILD_CARD,      /* * */
    DEEP_SCAN,      /* .. */
    NODE,           /* .child, ['child'] */
    EXPR_END,       /* ] */
    SLICE,          /* : */
    CHILD_SEP,      /* , */
    EXPR_START,     /* ? */
    STR,            /* "a string" 'a string' */
    EQ,             /* == */
    NEQ,            /* != */
    LT,             /* < */
    LTE,            /* <= */
    GT,             /* > */
    GTE,            /* >= */
    RGXP,           /* =~ */
    IN,             /* in */
    NIN,            /* nin */
    SIZE,           /*   */
    EMPTY,          /* empty */
    PAREN_OPEN,     /* ( */
    PAREN_CLOSE,    /* ) */
    LITERAL,        /* "some string" 'some string' */
    FILTER_START    /* [ */
} token;

extern const char * visible[];

token scan(char ** p, char * buffer, size_t bufSize);

#endif /* LEXER_H */
