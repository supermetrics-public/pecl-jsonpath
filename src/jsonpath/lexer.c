#include "lexer.h"

#include <string.h>
#ifndef S_SPLINT_S
#include <ctype.h>
#endif
#include <stdbool.h>
#include <stdio.h>

#include "safe_string.h"

static bool extract_quoted_literal(char* p, char* buffer, size_t bufSize, lex_error* err);
static bool extract_unbounded_literal(char* p, char* buffer, size_t bufSize, lex_error* err);
static bool extract_unbounded_numeric_literal(char* p, char* buffer, size_t bufSize, lex_error* err);
static bool extract_boolean_literal(char* p, char* buffer, size_t bufSize, lex_error* err);

const char* LEX_STR[] = {
    "LEX_NOT_FOUND",    /* Token not found */
    "LEX_ROOT",         /* $ */
    "LEX_CUR_NODE",     /* @ */
    "LEX_WILD_CARD",    /* * */
    "LEX_DEEP_SCAN",    /* .. */
    "LEX_NODE",         /* .child, ['child'] */
    "LEX_EXPR_END",     /* ] */
    "LEX_SLICE",        /* : */
    "LEX_CHILD_SEP",    /* , */
    "LEX_EXPR_START",   /* ? */
    "LEX_EQ",           /* == */
    "LEX_NEQ",          /* != */
    "LEX_LT",           /* < */
    "LEX_LTE",          /* <= */
    "LEX_GT",           /* > */
    "LEX_GTE",          /* >= */
    "LEX_RGXP",         /* =~ */
    "LEX_PAREN_OPEN",   /* ( */
    "LEX_PAREN_CLOSE",  /* ) */
    "LEX_LITERAL",      /* "some string" 'some string' */
    "LEX_LITERAL_BOOL", /* true, false */
    "LEX_FILTER_START", /* [ */
    "LEX_AND",          /* && */
    "LEX_OR",           /* || */
    "LEX_ERR"           /* Signals lexing error */
};

lex_token scan(char** p, char* buffer, size_t bufSize, lex_error* err) {
  char* start = *p;
  lex_token found_token = LEX_NOT_FOUND;

  while (**p != '\0' && found_token == LEX_NOT_FOUND) {
    switch (**p) {
      case '$':
        found_token = LEX_ROOT;
        break;
      case '.':

        switch (*(*p + 1)) {
          case '.':
            found_token = LEX_DEEP_SCAN;
            break;
          case '[':
            break; /* dot is superfluous in .['node'] */
          case '*':
            break; /* get in next loop */
          default:
            found_token = LEX_NODE;
            break;
        }

        if (found_token == LEX_NODE) {
          (*p)++;

          if (!extract_unbounded_literal(*p, buffer, bufSize, err)) {
            return LEX_ERR;
          }

          *p += strlen(buffer) - 1;
        }

        break;
      case '[':
        (*p)++;

        for (; **p == ' '; (*p)++)
          ;

        switch (**p) {
          case '\'':
            if (!extract_quoted_literal(*p, buffer, bufSize, err)) {
              return LEX_ERR;
            }
            *p += strlen(buffer) + 2;

            for (; **p == ' '; (*p)++)
              ;

            if (**p != ']') {
              err->pos = *p;
              strcpy(err->msg, "Missing closing ] bracket");
              return LEX_ERR;
            }
            found_token = LEX_NODE;
            break;
          case '"':
            if (!extract_quoted_literal(*p, buffer, bufSize, err)) {
              return LEX_ERR;
            }
            *p += strlen(buffer) + 2;

            for (; **p == ' '; (*p)++)
              ;

            if (**p != ']') {
              err->pos = *p;
              strcpy(err->msg, "Missing closing ] bracket");
              return LEX_ERR;
            }
            found_token = LEX_NODE;
            break;
          case '?':
            found_token = LEX_EXPR_START;
            break;
          default:
            /* Pick up start in next iteration, maybe simplify */
            (*p)--;
            found_token = LEX_FILTER_START;
            break;
        }
        break;
      case ']':
        found_token = LEX_EXPR_END;
        break;
      case '@':
        found_token = LEX_CUR_NODE;
        break;
      case ':':
        found_token = LEX_SLICE;
        break;
      case ',':
        found_token = LEX_CHILD_SEP;
        break;
      case '=':
        (*p)++;

        if (**p == '=') {
          found_token = LEX_EQ;
        } else if (**p == '~') {
          found_token = LEX_RGXP;
        }

        break;
      case '!':
        (*p)++;

        if (**p != '=') {
          err->pos = *p;
          strcpy(err->msg, "! operator missing =");
          return LEX_ERR;
        }

        found_token = LEX_NEQ;
        break;
      case '>':
        if (*(*p + 1) == '=') {
          found_token = LEX_GTE;
          (*p)++;
        } else {
          found_token = LEX_GT;
        }
        break;
      case '<':
        if (*(*p + 1) == '=') {
          found_token = LEX_LTE;
          (*p)++;
        } else {
          found_token = LEX_LT;
        }
        break;
      case '&':
        (*p)++;

        if (**p != '&') {
          err->pos = *p;
          strcpy(err->msg, "'And' operator must be double &&");
          return LEX_ERR;
        }

        found_token = LEX_AND;
        break;
      case '|':
        (*p)++;

        if (**p != '|') {
          err->pos = *p;
          strcpy(err->msg, "'Or' operator must be double ||");
          return LEX_ERR;
        }

        found_token = LEX_OR;
        break;
      case '(':
        found_token = LEX_PAREN_OPEN;
        break;
      case ')':
        found_token = LEX_PAREN_CLOSE;
        break;
      case '\'':
        if (!extract_quoted_literal(*p, buffer, bufSize, err)) {
          return LEX_ERR;
        }
        *p += strlen(buffer) + 1;
        found_token = LEX_LITERAL;
        break;
      case '"':
        if (!extract_quoted_literal(*p, buffer, bufSize, err)) {
          return LEX_ERR;
        }
        *p += strlen(buffer) + 1;
        found_token = LEX_LITERAL;
        break;
      case '*':
        found_token = LEX_WILD_CARD;
        break;
      case 't':
      case 'f':
        if (!extract_boolean_literal(*p, buffer, bufSize, err)) {
          return LEX_ERR;
        }
        *p += strlen(buffer) - 1;
        found_token = LEX_LITERAL_BOOL;
        break;
      case '-':
        if (!isdigit(*(*p + 1))) {
          return LEX_ERR;
        }
        if (!extract_unbounded_numeric_literal(*p, buffer, bufSize, err)) {
          return LEX_ERR;
        }
        *p += strlen(buffer) - 1;
        found_token = LEX_LITERAL;
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (!extract_unbounded_numeric_literal(*p, buffer, bufSize, err)) {
          return LEX_ERR;
        }
        *p += strlen(buffer) - 1;
        found_token = LEX_LITERAL;
        break;
    }

    (*p)++;
  }

  return found_token;
}

/* Extract contents of string bounded by either single or double quotes */
static bool extract_quoted_literal(char* p, char* buffer, size_t bufSize, lex_error* err) {
  char* start;
  char quote_type;
  bool quote_found = false;
  size_t cpy_len;

  for (; *p != '\0' && (*p == '\'' || *p == '"' || *p == ' '); p++) {
    // Find first occurrence
    if (*p == '\'' || *p == '"') {
      quote_found = true;
      quote_type = *p;
      p++;
      break;
    }
  }

  if (quote_found == false) {
    err->pos = p;
    strcpy(err->msg, "Missing opening quote in string literal");
    return false;
  }

  start = p;

  for (; *p != '\0' && *p != quote_type && *(p - 1) != '\\'; p++)
    ;

  cpy_len = (size_t)(p - start);

  if (jp_str_cpy(buffer, bufSize, start, cpy_len) > 0) {
    err->pos = p;
    sprintf(err->msg, "String size exceeded %zu bytes", bufSize);
    return false;
  }

  return true;
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static bool extract_unbounded_literal(char* p, char* buffer, size_t bufSize, lex_error* err) {
  char* start;
  size_t cpy_len;

  for (; *p == ' '; p++)
    ;

  start = p;

  for (; *p != '\0' && !isspace(*p) && (*p == '_' || *p == '-' || !ispunct(*p)); p++)
    ;

  cpy_len = (size_t)(p - start);

  if (jp_str_cpy(buffer, bufSize, start, cpy_len) > 0) {
    err->pos = p;
    sprintf(err->msg, "String size exceeded %zu bytes", bufSize);
    return false;
  }

  return true;
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static bool extract_unbounded_numeric_literal(char* p, char* buffer, size_t bufSize, lex_error* err) {
  char* start;
  size_t cpy_len;

  for (; *p == ' '; p++)
    ;

  start = p;

  if (*p == '-') {
    p++;
  }

  for (; isdigit(*p); p++)
    ;

  // Optional decimal separator and fraction part
  if (*p != '\0' && *(p + 1) != '\0' && *p == '.' && isdigit(*(p + 1))) {
    p++;

    for (; isdigit(*p); p++)
      ;
  }

  cpy_len = (size_t)(p - start);

  if (jp_str_cpy(buffer, bufSize, start, cpy_len) > 0) {
    err->pos = p;
    sprintf(err->msg, "String size exceeded %zu bytes", bufSize);
    return false;
  }

  return true;
}

/* Extract boolean */
static bool extract_boolean_literal(char* p, char* buffer, size_t bufSize, lex_error* err) {
  char* start;
  size_t cpy_len;

  for (; *p == ' '; p++)
    ;

  start = p;

  for (; *p != '\0' && !isspace(*p) && (*p == '_' || *p == '-' || !ispunct(*p)); p++)
    ;

  cpy_len = (size_t)(p - start);

  if (jp_str_cpy(buffer, bufSize, start, cpy_len) > 0) {
    err->pos = p;
    sprintf(err->msg, "String size exceeded %zu bytes", bufSize);
    return false;
  }

  return true;
}