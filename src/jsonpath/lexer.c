#include "lexer.h"

#include <string.h>

#include <ext/spl/spl_exceptions.h>

#include "php.h"
#include "zend_exceptions.h"
#ifndef S_SPLINT_S
#include <ctype.h>
#endif
#include <stdio.h>

#include "safe_string.h"

#define CUR_CHAR() **p
#define EAT_WHITESPACE()     \
  for (; **p == ' '; (*p)++)
#define PEEK_CHAR() *(*p + 1)
#define NEXT_CHAR() (*p)++

static int count_numeric_str_len(char* p);
static bool extract_quoted_literal(char** p, char* json_path, struct jpath_token* token);
static void extract_unbounded_literal(char** p, char* json_path, struct jpath_token* token);
static bool extract_unbounded_numeric_literal(char** p, char* json_path, struct jpath_token* token);
static bool extract_boolean_literal(char** p, char* json_path, struct jpath_token* token);

const char* LEX_STR[] = {
    "LEX_NOT_FOUND",       /* Token not found */
    "LEX_ROOT",            /* $ */
    "LEX_CUR_NODE",        /* @ */
    "LEX_WILD_CARD",       /* * */
    "LEX_DEEP_SCAN",       /* .. */
    "LEX_NODE",            /* .child, ['child'] */
    "LEX_EXPR_END",        /* ] */
    "LEX_SLICE",           /* : */
    "LEX_CHILD_SEP",       /* , */
    "LEX_EXPR_START",      /* ? */
    "LEX_EQ",              /* == */
    "LEX_NEQ",             /* != */
    "LEX_LT",              /* < */
    "LEX_LTE",             /* <= */
    "LEX_GT",              /* > */
    "LEX_GTE",             /* >= */
    "LEX_RGXP",            /* =~ */
    "LEX_PAREN_OPEN",      /* ( */
    "LEX_PAREN_CLOSE",     /* ) */
    "LEX_LITERAL",         /* "some string" 'some string' */
    "LEX_LITERAL_BOOL",    /* true, false */
    "LEX_LITERAL_NUMERIC", /* long, double */
    "LEX_FILTER_START",    /* [ */
    "LEX_AND",             /* && */
    "LEX_OR",              /* || */
    "LEX_NEGATION",        /* !@.value */
    "LEX_ERR"              /* Signals lexing error */
};

void raise_error(const char* msg, char* json_path, char* cur_pos) {
  zend_throw_exception_ex(spl_ce_RuntimeException, 0, "%s at position %ld", msg, (cur_pos - json_path));
}

bool scan(char** p, struct jpath_token* token, char* json_path) {
  char* start = *p;

  token->len = 0;
  token->type = LEX_NOT_FOUND;
  token->val = NULL;

  while (CUR_CHAR() != '\0' && token->type == LEX_NOT_FOUND) {
    switch (CUR_CHAR()) {
      case '$':
        token->type = LEX_ROOT;
        NEXT_CHAR();
        return true;
      case '.':
        NEXT_CHAR();
        switch (CUR_CHAR()) {
          case '.':
            token->type = LEX_DEEP_SCAN;
            return true;
          case '[':
            break; /* dot is superfluous in .['node'] */
          case '*':
            break; /* get in next loop */
          default:
            token->type = LEX_NODE;
            extract_unbounded_literal(p, json_path, token);
            return true;
        }
        break;
      // case '[':
      //   NEXT_CHAR();
      //   EAT_WHITESPACE();

      //   switch (CUR_CHAR()) {
      //     case '\'':

      //       if (!extract_quoted_literal(p, json_path, token)) {
      //         return false;
      //       }

      //       token->type = LEX_NODE;

      //       EAT_WHITESPACE();

      //       if (CUR_CHAR() != ']') {
      //         raise_error("Missing closing ] bracket", json_path, *p);
      //         return false;
      //       }
      //       NEXT_CHAR();
      //       return true;
      //     case '"':
      //       if (!extract_quoted_literal(p, json_path, token)) {
      //         return false;
      //       }

      //       EAT_WHITESPACE();

      //       if (CUR_CHAR() != ']') {
      //         raise_error("Missing closing ] bracket", json_path, *p);
      //         return false;
      //       }
      //       token->type = LEX_NODE;
      //       break;
      //     case '?':
      //       token->type = LEX_EXPR_START;
      //       break;
      //     default:
      //       /* Pick up start in next iteration, maybe simplify */
      //       (*p)--;
      //       token->type = LEX_FILTER_START;
      //       break;
      //   }
      //   NEXT_CHAR();
      //   break;
      // case ']':
      //   token->type = LEX_EXPR_END;
      //   NEXT_CHAR();
      //   break;
      // case '@':
      //   token->type = LEX_CUR_NODE;
      //   NEXT_CHAR();
      //   break;
      // case ':':
      //   token->type = LEX_SLICE;
      //   NEXT_CHAR();
      //   break;
      // case ',':
      //   token->type = LEX_CHILD_SEP;
      //   NEXT_CHAR();
      //   break;
      // case '=':
      //   NEXT_CHAR();

      //   if (CUR_CHAR() == '=') {
      //     token->type = LEX_EQ;
      //   } else if (CUR_CHAR() == '~') {
      //     token->type = LEX_RGXP;
      //   } else {
      //     raise_error("Invalid character after '='. Valid values: '==', '!~'.", json_path, *p);
      //     return false;
      //   }
      //   NEXT_CHAR();
      //   break;
      // case '!':
      //   if (*(*p + 1) == '=') {
      //     token->type = LEX_NEQ;
      //     NEXT_CHAR();
      //   } else {
      //     token->type = LEX_NEGATION;
      //   }
      //   NEXT_CHAR();
      //   break;
      // case '>':
      //   if (*(*p + 1) == '=') {
      //     token->type = LEX_GTE;
      //     NEXT_CHAR();
      //   } else {
      //     token->type = LEX_GT;
      //   }
      //   NEXT_CHAR();
      //   break;
      // case '<':
      //   if (*(*p + 1) == '=') {
      //     token->type = LEX_LTE;
      //     NEXT_CHAR();
      //   } else {
      //     token->type = LEX_LT;
      //   }
      //   NEXT_CHAR();
      //   break;
      // case '&':
      //   NEXT_CHAR();

      //   if (CUR_CHAR() != '&') {
      //     raise_error("'And' operator must be double &&", json_path, *p);
      //     return false;
      //   }

      //   token->type = LEX_AND;
      //   NEXT_CHAR();
      //   break;
      // case '|':
      //   NEXT_CHAR();

      //   if (CUR_CHAR() != '|') {
      //     raise_error("'Or' operator must be double ||", json_path, *p);
      //     return false;
      //   }

      //   token->type = LEX_OR;
      //   NEXT_CHAR();
      //   break;
      // case '(':
      //   token->type = LEX_PAREN_OPEN;
      //   NEXT_CHAR();
      //   break;
      // case ')':
      //   token->type = LEX_PAREN_CLOSE;
      //   NEXT_CHAR();
      //   break;
      // case '\'':
      //   if (!extract_quoted_literal(p, json_path, token)) {
      //     return false;
      //   }
      //   token->type = LEX_LITERAL;
      //   NEXT_CHAR();
      //   break;
      // case '"':
      //   if (!extract_quoted_literal(p, json_path, token)) {
      //     return false;
      //   }
      //   token->type = LEX_LITERAL;
      //   NEXT_CHAR();
      //   break;
      // case '*':
      //   token->type = LEX_WILD_CARD;
      //   NEXT_CHAR();
      //   break;
      // case 't':
      // case 'T':
      // case 'f':
      // case 'F':
      //   if (!extract_boolean_literal(p, json_path, token)) {
      //     return false;
      //   }
      //   token->type = LEX_LITERAL_BOOL;
      //   break;
      // case '-':
      // case '0':
      // case '1':
      // case '2':
      // case '3':
      // case '4':
      // case '5':
      // case '6':
      // case '7':
      // case '8':
      // case '9':
      //   return extract_unbounded_numeric_literal(p, json_path, token);
      case ' ':
        NEXT_CHAR();
        break;
      default:
        zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unrecognized token '%c' at position %ld", CUR_CHAR(),
                                (*p - json_path));
        return false;
    }
  }

  return false;
}

/* Extract contents of string bounded by either single or double quotes */
static bool extract_quoted_literal(char** p, char* json_path, struct jpath_token* token) {
  char quote_type;
  bool quote_found = false;
  char* start = *p;

  for (; CUR_CHAR() != '\0' && (CUR_CHAR() == '\'' || CUR_CHAR() == '"' || CUR_CHAR() == ' '); NEXT_CHAR()) {
    // Find first occurrence
    if (CUR_CHAR() == '\'' || CUR_CHAR() == '"') {
      quote_found = true;
      quote_type = CUR_CHAR();
      NEXT_CHAR();
      break;
    }
  }

  if (quote_found == false) {
    raise_error("Missing opening quote in string literal", json_path, start);
    return false;
  }

  token->len = 0;
  token->val = *p;
  token->type = LEX_LITERAL;

  for (; CUR_CHAR() != '\0' && CUR_CHAR() != quote_type && *(*p - 1) != '\\'; NEXT_CHAR()) {
    token->len++;
  }

  NEXT_CHAR();

  return true;
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static void extract_unbounded_literal(char** p, char* json_path, struct jpath_token* token) {
  for (; CUR_CHAR() == ' '; NEXT_CHAR())
    ;

  token->len = 0;
  token->val = *p;

  for (; CUR_CHAR() != '\0' && !isspace(CUR_CHAR()) && (CUR_CHAR() == '_' || CUR_CHAR() == '-' || !ispunct(CUR_CHAR())); NEXT_CHAR()) {
    token->len++;
  }
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static bool extract_unbounded_numeric_literal(char** p, char* json_path, struct jpath_token* token) {
  token->len = 0;
  token->type = LEX_LITERAL_NUMERIC;
  token->val = *p;

  while (CUR_CHAR() != '\0') {
    if ((CUR_CHAR() >= '0' && CUR_CHAR() <= '9') || CUR_CHAR() == '.' || CUR_CHAR() == '-' || CUR_CHAR() == 'e' || CUR_CHAR() == 'E') {
      token->len++;
    } else {
      break;
    }
    NEXT_CHAR();
  }

  return true;
}

/* Extract boolean */
static bool extract_boolean_literal(char** p, char* json_path, struct jpath_token* token) {
  char* start;
  size_t cpy_len;

  for (; CUR_CHAR() == ' '; NEXT_CHAR())
    ;

  start = *p;

  for (; CUR_CHAR() != '\0' && !isspace(CUR_CHAR()) && (CUR_CHAR() == '_' || CUR_CHAR() == '-' || !ispunct(CUR_CHAR())); NEXT_CHAR())
    ;

  cpy_len = (size_t)(*p - start);

  token->len = cpy_len;
  token->type = LEX_LITERAL;
  token->val = start;

  return true;
}