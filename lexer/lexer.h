#ifndef LEXER_H
#define LEXER_H 1

#include <stdio.h>

typedef enum {
    ROOT,           /* $ */
    CUR_NODE,       /* @ */
    WILD_CARD,      /* * */
    DEEP_SCAN,      /* .. */
    CHILD_NODE,     /* .child, ['child'] */
    RIGHT_SQ,       /* ] */
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
    LITERAL         /* "some string" 'some string' */
} token;

extern const char * visible[];

token scan(char ** p, char * buffer, size_t bufSize);

#endif /* LEXER_H */