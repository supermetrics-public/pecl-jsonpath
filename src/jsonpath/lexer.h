#ifndef LEXER_H
#define LEXER_H 1

#include <stddef.h>

typedef enum {
  LEX_AND,             /* && */
  LEX_CHILD_SEP,       /* , */
  LEX_CUR_NODE,        /* @ */
  LEX_DEEP_SCAN,       /* .. */
  LEX_EQ,              /* == */
  LEX_ERR,             /* Signals lexing error */
  LEX_EXPR_END,        /* ] */
  LEX_EXPR_START,      /* ? */
  LEX_FILTER_START,    /* [ */
  LEX_GT,              /* > */
  LEX_GTE,             /* >= */
  LEX_LITERAL_BOOL,    /* true, false */
  LEX_LITERAL_NUMERIC, /* int, float */
  LEX_LITERAL,         /* "some string" 'some string' */
  LEX_LT,              /* < */
  LEX_LTE,             /* <= */
  LEX_NEGATION,        /* !@.value */
  LEX_NEQ,             /* != */
  LEX_NODE,            /* .child, ['child'] */
  LEX_NOT_FOUND,       /* Token not found */
  LEX_OR,              /* || */
  LEX_PAREN_CLOSE,     /* ) */
  LEX_PAREN_OPEN,      /* ( */
  LEX_RGXP,            /* =~ */
  LEX_ROOT,            /* $ */
  LEX_SLICE,           /* : */
  LEX_WILD_CARD        /* * */
} lex_token;

extern const char* LEX_STR[];

lex_token scan(char** p, char* buffer, size_t bufSize, char* json_path);

#endif /* LEXER_H */
