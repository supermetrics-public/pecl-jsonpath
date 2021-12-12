#include "lexer.h"

#include <string.h>
#ifndef S_SPLINT_S
#include <ctype.h>
#endif

#include "exceptions.h"

#define CUR_CHAR() **p
#define NEXT_CHAR() (*p)++
#define EAT_WHITESPACE() for (; CUR_CHAR() == ' '; NEXT_CHAR())
#define PEEK_CHAR() *(*p + 1)

static bool extract_quoted_literal(char** p, char* json_path, struct jpath_token* token);
static bool extract_regex(char** p, char* json_path, struct jpath_token* token);
static void extract_boolean_or_null_literal(char** p, struct jpath_token* token);
static void extract_unbounded_literal(char** p, struct jpath_token* token);
static void extract_unbounded_numeric_literal(char** p, struct jpath_token* token);
static void raise_error(const char* msg, char* json_path, char* cur_pos);

const char* LEX_STR[] = {
    "LEX_AND",             /* && */
    "LEX_CHILD_SEP",       /* , */
    "LEX_CUR_NODE",        /* @ */
    "LEX_DEEP_SCAN",       /* .. */
    "LEX_EQ",              /* == */
    "LEX_EXPR_END",        /* ] */
    "LEX_EXPR_START",      /* ? */
    "LEX_FILTER_START",    /* [ */
    "LEX_GT",              /* > */
    "LEX_GTE",             /* >= */
    "LEX_LITERAL",         /* "some string" 'some string' */
    "LEX_LITERAL_BOOL",    /* true, false */
    "LEX_LITERAL_NULL",    /* null */
    "LEX_LITERAL_NUMERIC", /* long, double */
    "LEX_LT",              /* < */
    "LEX_LTE",             /* <= */
    "LEX_NEGATION",        /* !@.value */
    "LEX_NEQ",             /* != */
    "LEX_NODE",            /* .child, ['child'] */
    "LEX_NOT_FOUND",       /* Token not found */
    "LEX_OR",              /* || */
    "LEX_PAREN_CLOSE",     /* ) */
    "LEX_PAREN_OPEN",      /* ( */
    "LEX_RGXP",            /* =~ */
    "LEX_ROOT",            /* $ */
    "LEX_SLICE",           /* : */
    "LEX_WILD_CARD",       /* * */
};

bool scan(char** p, struct jpath_token* token, char* json_path) {
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
            token->type = LEX_WILD_CARD;
            NEXT_CHAR();
            return true;
          case ' ':
            raise_error("Unexpected whitespace", json_path, *p);
            return false;
          case '\0':
            if (*(*p - 2) == '.') {
              raise_error("Recursive descent operator `..` must be followed by a child selector, filter or wildcard",
                          json_path, *p);
              return false;
            }

            raise_error("Dot selector `.` must be followed by a node name or wildcard", json_path, *p - 1);
            return false;
          default:
            if (CUR_CHAR() == '"' || CUR_CHAR() == '\'') {
              raise_error("Quoted node names must use the bracket notation `[`", json_path, *p);
              return false;
            }
            extract_unbounded_literal(p, token);
            token->type = LEX_NODE;
            return true;
        }
        break;
      case '[':
        NEXT_CHAR();
        EAT_WHITESPACE();

        switch (CUR_CHAR()) {
          case '?':
            token->type = LEX_EXPR_START;
            NEXT_CHAR();
            return true;
          case '*':
            token->type = LEX_WILD_CARD;
            NEXT_CHAR();
            if (CUR_CHAR() != ']') {
              raise_error("Wildcard filter contains an invalid character, expected `]`", json_path, *p);
              return false;
            }
            NEXT_CHAR();
            return true;
          default:
            token->type = LEX_FILTER_START;
            return true;
        }
      case ']':
        token->type = LEX_EXPR_END;
        NEXT_CHAR();
        return true;
      case '@':
        token->type = LEX_CUR_NODE;
        NEXT_CHAR();
        return true;
      case ':':
        token->type = LEX_SLICE;
        NEXT_CHAR();
        return true;
      case ',':
        token->type = LEX_CHILD_SEP;
        NEXT_CHAR();
        return true;
      case '=':
        NEXT_CHAR();

        if (CUR_CHAR() == '=') {
          token->type = LEX_EQ;
        } else if (CUR_CHAR() == '~') {
          token->type = LEX_RGXP;
        } else {
          raise_error("Invalid character after `=`, valid values are `==` and `=~`", json_path, *p);
          return false;
        }
        NEXT_CHAR();
        return true;
      case '!':
        if (PEEK_CHAR() == '=') {
          token->type = LEX_NEQ;
          NEXT_CHAR();
        } else {
          token->type = LEX_NEGATION;
        }
        NEXT_CHAR();
        return true;
      case '>':
        if (PEEK_CHAR() == '=') {
          token->type = LEX_GTE;
          NEXT_CHAR();
        } else {
          token->type = LEX_GT;
        }
        NEXT_CHAR();
        return true;
      case '<':
        if (PEEK_CHAR() == '=') {
          token->type = LEX_LTE;
          NEXT_CHAR();
        } else {
          token->type = LEX_LT;
        }
        NEXT_CHAR();
        return true;
      case '&':
        NEXT_CHAR();

        if (CUR_CHAR() != '&') {
          raise_error("AND operator must be double ampersand `&&`", json_path, *p);
          return false;
        }

        token->type = LEX_AND;
        NEXT_CHAR();
        return true;
      case '|':
        NEXT_CHAR();

        if (CUR_CHAR() != '|') {
          raise_error("OR operator must be double pipe `||`", json_path, *p);
          return false;
        }

        token->type = LEX_OR;
        NEXT_CHAR();
        return true;
      case '(':
        token->type = LEX_PAREN_OPEN;
        NEXT_CHAR();
        return true;
      case ')':
        token->type = LEX_PAREN_CLOSE;
        NEXT_CHAR();
        return true;
      case '\'':
      case '"':
        if (!extract_quoted_literal(p, json_path, token)) {
          return false;
        }
        token->type = LEX_LITERAL;
        return true;
      case 't':
      case 'T':
      case 'f':
      case 'F':
        extract_boolean_or_null_literal(p, token);
        token->type = LEX_LITERAL_BOOL;
        return true;
      case 'n':
      case 'N':
        extract_boolean_or_null_literal(p, token);
        token->type = LEX_LITERAL_NULL;
        return true;
      case '/':
        if (!extract_regex(p, json_path, token)) {
          return false;
        }
        token->type = LEX_LITERAL;
        return true;
      case '-':
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
        extract_unbounded_numeric_literal(p, token);
        return true;
      case ' ':
        NEXT_CHAR();
        break;
      default:
        throw_jsonpath_exception("Unrecognized token `%c` at position %ld", CUR_CHAR(), (*p - json_path));
        return false;
    }
  }

  return false;
}

static void raise_error(const char* msg, const char* json_path, const char* cur_pos) {
  throw_jsonpath_exception("%s at position %ld", msg, (cur_pos - json_path));
}

/* Extract contents of string bounded by either single or double quotes */
static bool extract_quoted_literal(char** p, char* json_path, struct jpath_token* token) {
  char quote_type;
  bool quote_found = false;
  char* start = *p;

  for (; (CUR_CHAR() == '\'' || CUR_CHAR() == '"' || CUR_CHAR() == ' '); NEXT_CHAR()) {
    /* Find first occurrence of single or double quote */
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

  while (CUR_CHAR() != '\0') {
    /* Find escaped quotes and backslashes and remove the escaping by shifting the string */
    if (CUR_CHAR() == '\\') {
      if (PEEK_CHAR() == '\\' || PEEK_CHAR() == '\'' || PEEK_CHAR() == '"') {
        memmove(*p, *p + 1, strlen(*p));
        token->len++;
        NEXT_CHAR();
        continue;
      }
    }

    if (CUR_CHAR() == quote_type) {
      break;
    }

    token->len++;
    NEXT_CHAR();
  }

  NEXT_CHAR();

  return true;
}

static bool extract_regex(char** p, char* json_path, struct jpath_token* token) {
  token->len = 0;
  token->val = *p;
  char* start = *p;

  /* consume opening slash */
  token->len++;
  NEXT_CHAR();

  for (; CUR_CHAR() != '\0' && CUR_CHAR() != '/'; NEXT_CHAR()) {
    if (CUR_CHAR() == '\\' && PEEK_CHAR() == '/') {
      /* skip escaped forward slash */
      token->len++;
      NEXT_CHAR();
    }
    token->len++;
  }

  if (CUR_CHAR() != '/') {
    raise_error("Missing closing regex /", json_path, start);
    return false;
  }

  /* consume closing slash */
  token->len++;
  NEXT_CHAR();

  return true;
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static void extract_unbounded_literal(char** p, struct jpath_token* token) {
  token->len = 0;
  token->val = *p;

  EAT_WHITESPACE();

  for (; CUR_CHAR() != '\0' && !isspace(CUR_CHAR()) && (CUR_CHAR() == '_' || CUR_CHAR() == '-' || !ispunct(CUR_CHAR()));
       NEXT_CHAR()) {
    token->len++;
  }
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static void extract_unbounded_numeric_literal(char** p, struct jpath_token* token) {
  token->len = 0;
  token->type = LEX_LITERAL_NUMERIC;
  token->val = *p;

  for (; (CUR_CHAR() >= '0' && CUR_CHAR() <= '9') || CUR_CHAR() == '.' || CUR_CHAR() == '-' || CUR_CHAR() == 'e' ||
         CUR_CHAR() == 'E';
       NEXT_CHAR()) {
    token->len++;
  }
}

/* Extract boolean or null */
static void extract_boolean_or_null_literal(char** p, struct jpath_token* token) {
  token->len = 0;
  token->type = LEX_LITERAL;
  token->val = *p;

  for (; CUR_CHAR() != '\0' && !isspace(CUR_CHAR()) && (CUR_CHAR() == '_' || CUR_CHAR() == '-' || !ispunct(CUR_CHAR()));
       NEXT_CHAR()) {
    token->len++;
  }
}