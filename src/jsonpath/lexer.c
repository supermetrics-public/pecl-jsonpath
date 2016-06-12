#include "lexer.h"
#include <string.h>
#ifndef S_SPLINT_S
#include <ctype.h>
#endif
static void extract_quoted_literal(char *p, char * buffer, size_t bufSize);
static void extract_unbounded_literal(char *p, char * buffer, size_t bufSize);
static void extract_unbounded_numeric_literal(char *p, char * buffer, size_t bufSize);

const char * visible[] = {
    "NOT_FOUND",      /* Token not found */
    "ROOT",           /* $ */
    "CUR_NODE",       /* @ */
    "WILD_CARD",      /* * */
    "DEEP_SCAN",      /* .. */
    "NODE",           /* .child, ['child'] */
    "EXPR_END",       /* ] */
    "SLICE",          /* : */
    "CHILD_SEP",      /* , */
    "EXPR_START",     /* ? */
    "EQ",             /* == */
    "NEQ",            /* != */
    "LT",             /* < */
    "LTE",            /* <= */
    "GT",             /* > */
    "GTE",            /* >= */
    "RGXP",           /* =~ */
    "PAREN_OPEN",     /* ( */
    "PAREN_CLOSE",    /* ) */
    "LITERAL",        /* \"some string\" 'some string' */
    "FILTER_START",   /* [ */
};

lex_token scan(char ** p, char * buffer, size_t bufSize) {

    lex_token found_token = LEX_NOT_FOUND;

    while(**p != '\0' && found_token == LEX_NOT_FOUND) {

        switch(**p) {

            case '$':
                found_token = LEX_ROOT;
                break;
            case '.':

                switch(*(*p+1)) {
                    case '.':
                        found_token = LEX_DEEP_SCAN;
                        break;
                    case ' ':  /* space is invalid in . ['node'] */
                        /* Throw parsing error */
                        break;
                    case '[':
                        break;  /* dot is superfluous in .['node'] */
                    case '*':
                        break; /* get in next loop */
                    default:
                        found_token = LEX_NODE;
                        break;

                }

                if(found_token == LEX_NODE) {
                    (*p)++;

                    extract_unbounded_literal(*p, buffer, bufSize);

                    *p += strlen(buffer) - 1;
                }

                break;
            case '[':
                (*p)++;

                for (; **p != '\0' && **p == ' '; (*p)++);

                switch(**p) {
                    case '\'':
                        extract_quoted_literal(*p, buffer, bufSize);
                        *p += strlen(buffer) + 2;

                        for (; **p != '\0' && **p == ' '; (*p)++);

                        if(**p != ']') {
                            //Lexing error
                        }
                        found_token = LEX_NODE;
                        break;
                    case '"':
                        extract_quoted_literal(*p, buffer, bufSize);
                        *p += strlen(buffer) + 2;

                        for (; **p != '\0' && **p == ' '; (*p)++);

                        if(**p != ']') {
                            //Lexing error
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

                if(**p == '=') {
                    found_token = LEX_EQ;
                } else if(**p == '~') {
                    found_token = LEX_RGXP;
                }

                break;
            case '!':
                (*p)++;

                if(**p != '=') {
                    //Lexing error
                }

                found_token = LEX_NEQ;
                break;
            case '>':
                if(*(*p+1) == '=') {
                    found_token = LEX_GTE;
                    (*p)++;
                } else {
                    found_token = LEX_GT;
                }
                break;
            case '<':
                if(*(*p+1) == '=') {
                    found_token = LEX_LTE;
                    (*p)++;
                } else {
                    found_token = LEX_LT;
                }
                break;
            case '&':
                if(*(*p+1) == '&') {
                    found_token = LEX_AND;
                    (*p)++;
                } else {
                    /* Lexing error */
                }
                break;
            case '|':
                if(*(*p+1) == '|') {
                    found_token = LEX_OR;
                    (*p)++;
                } else {
                    /* Lexing error */
                }
                break;
            case '(':
                found_token = LEX_PAREN_OPEN;
                break;
            case ')':
                found_token = LEX_PAREN_CLOSE;
                break;
            case '\'':
                extract_quoted_literal(*p, buffer, bufSize);
                *p += strlen(buffer) + 1;
                found_token = LEX_LITERAL;
                break;
            case '"':
                extract_quoted_literal(*p, buffer, bufSize);
                *p += strlen(buffer) + 1;
                found_token = LEX_LITERAL;
                break;
            case '*':
                found_token = LEX_WILD_CARD;
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
                extract_unbounded_numeric_literal(*p, buffer, bufSize);
                *p += strlen(buffer) - 1;
                found_token = LEX_LITERAL;
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

    for (; *p != '\0' && !isspace(*p) && !ispunct(*p); p++);

    cpy_len = (size_t) (p - start);

    if(cpy_len > (size_t) bufSize - 1) {
        printf("Error: Exceeded max node name length of %d\n", (int)bufSize - 1);
        return;
    }

    strncpy(buffer, start, cpy_len);
    buffer[cpy_len] = '\0';
}

/* Extract literal without clear bounds that ends in non alpha-numeric char */
static void extract_unbounded_numeric_literal(char *p, char * buffer, size_t bufSize) {

    char * start;
    size_t cpy_len;

    for (; *p != '\0' && *p == ' '; p++);

    start = p;

    for (; isdigit(*p); p++);

    cpy_len = (size_t) (p - start);

    if(cpy_len > (size_t) bufSize - 1) {
        printf("Error: Exceeded max node name length of %d\n", (int)bufSize - 1);
        return;
    }

    strncpy(buffer, start, cpy_len);
    buffer[cpy_len] = '\0';
}
