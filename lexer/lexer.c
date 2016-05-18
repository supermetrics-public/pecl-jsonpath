#include "lexer.h"
#include <string.h>
#include <ctype.h>

static void extract_quoted_literal(char *p, char * buffer, size_t bufSize);
static void extract_unbounded_literal(char *p, char * buffer, size_t bufSize);

const char * visible[] = {
    "ROOT,          /* $ */",
    "CUR_NODE,      /* @ */",
    "WILD_CARD,     /* * */",
    "DEEP_SCAN,     /* .. */",
    "CHILD_NODE,    /* .child, ['child'] */",
    "RIGHT_SQ,      /* ] */",
    "SLICE,         /* : */",
    "CHILD_SEP,     /* , */",
    "EXPR_START,    /* ? */",
    "STR,           /* \"a string\" 'a string' */",
    "EQ,            /* == */",
    "NEQ,           /* != */",
    "LT,            /* < */",
    "LTE,           /* <= */",
    "GT,            /* > */",
    "GTE,           /* >= */",
    "RGXP,          /* =~ */",
    "IN,            /* in */",
    "NIN,           /* nin */",
    "SIZE,          /*   */",
    "EMPTY,         /* empty */",
    "PAREN_OPEN,    /* ( * /",
    "PAREN_CLOSE,   /* ) * /",
    "LITERAL,       /* \"some string\" 'some string' */"
};

token scan(char ** p, char * buffer, size_t bufSize) {

    token found_token = -1;

    while(**p != '\0' && found_token == -1) {

        switch(**p) {

            case '$':
                found_token = ROOT;
                break;
            case '.':
                if(*(*p+1) == '.') {
                    found_token = DEEP_SCAN;
                } else {
                    (*p)++;
                    extract_unbounded_literal(*p, buffer, bufSize);
                    found_token = CHILD_NODE;
                }
                //TODO some exception for deep scanning on bracket ..['']
                break;
            case '[':
                (*p)++;

                for (; **p != '\0' && **p == ' '; (*p)++);

                switch(**p) {
                    case '\'':
                        extract_quoted_literal(*p, buffer, bufSize);
                        found_token = CHILD_NODE;
                        break;
                    case '"':
                        extract_quoted_literal(*p, buffer, bufSize);
                        found_token = CHILD_NODE;
                        break;
                    case '?':
                        found_token = EXPR_START;
                        break;
                }
                break;
            case ']':
                found_token = RIGHT_SQ;
                break;
            case '@':
                found_token = CUR_NODE;
                break;
            case ':':
                found_token = SLICE;
                break;
            case ',':
                found_token = CHILD_SEP;
                break;
            case '=':
                (*p)++;

                if(**p == '=') {
                    found_token = EQ;
                } else if(**p == '~') {
                    found_token = RGXP;
                } else {
                    //Lexing error
                }

                break;
            case '!':
                (*p)++;

                if(**p != '=') {
                    //Lexing error
                }

                found_token = NEQ;
                break;
            case '>':
                if(*(*p+1) == '=') {
                    found_token = GTE;
                    (*p)++;
                } else {
                    found_token = GT;
                }
                break;
            case '<':
                if(*(*p+1) == '=') {
                    found_token = LTE;
                    (*p)++;
                } else {
                    found_token = LT;
                }
                break;
            case '(':
                found_token = PAREN_OPEN;
                break;
            case ')':
                found_token = PAREN_CLOSE;
                break;
            case '\'':
                extract_quoted_literal(*p, buffer, bufSize);
                found_token = LITERAL;
                break;
            case '"':
                extract_quoted_literal(*p, buffer, bufSize);
                found_token = LITERAL;
                break;
        }

        (*p)++;
    }

    return found_token;
}

/* Extract contents of string bounded by single or double quotes */
static void extract_quoted_literal(char *p, char * buffer, size_t bufSize) {

    char * start;
    size_t cpy_len;

    for (; *p != '\0' && (*p == '\'' || *p == '"' || *p == ' '); p++);

    start = p;

    for (; *p != '\0' && (*p != '\'' && *p != '"'); p++);

    cpy_len = (size_t) (p - start);

    if(cpy_len > (size_t) bufSize - 1) {
        printf("Error: Exceeded max node name length of %d\n", (int)bufSize - 1);
        return;
    }

    strncpy(buffer, start, cpy_len);
    buffer[cpy_len] = '\0';
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static void extract_unbounded_literal(char *p, char * buffer, size_t bufSize) {

    char * start;
    size_t cpy_len;

    for (; *p != '\0' && *p == ' '; p++);

    start = p;

    for (; *p != '\0' && (isalnum(*p) || *p == '_' || *p == '-'); p++);

    cpy_len = (size_t) (p - start);

    if(cpy_len > (size_t) bufSize - 1) {
        printf("Error: Exceeded max node name length of %d\n", (int)bufSize - 1);
        return;
    }

    strncpy(buffer, start, cpy_len);
    buffer[cpy_len] = '\0';
}