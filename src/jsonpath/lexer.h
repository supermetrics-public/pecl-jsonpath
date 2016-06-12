#ifndef LEXER_H
#define LEXER_H 1

#include <stdio.h>

typedef enum {
    LEX_NOT_FOUND,      /* Token not found */
    LEX_ROOT,           /* $ */
    LEX_CUR_NODE,       /* @ */
    LEX_WILD_CARD,      /* * */
    LEX_DEEP_SCAN,      /* .. */
    LEX_NODE,           /* .child, ['child'] */
    LEX_EXPR_END,       /* ] */
    LEX_SLICE,          /* : */
    LEX_CHILD_SEP,      /* , */
    LEX_EXPR_START,     /* ? */
    LEX_EQ,             /* == */
    LEX_NEQ,            /* != */
    LEX_LT,             /* < */
    LEX_LTE,            /* <= */
    LEX_GT,             /* > */
    LEX_GTE,            /* >= */
    LEX_RGXP,           /* =~ */
    LEX_PAREN_OPEN,     /* ( */
    LEX_PAREN_CLOSE,    /* ) */
    LEX_LITERAL,        /* "some string" 'some string' */
    LEX_FILTER_START,   /* [ */
    LEX_AND,            /* && */
    LEX_OR              /* || */
} lex_token;

extern const char * visible[];

lex_token scan(char ** p, char * buffer, size_t bufSize);

#endif /* LEXER_H */
