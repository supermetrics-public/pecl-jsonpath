#include "lexer.h"
#include <string.h>
#include <ctype.h>

static void extract_quoted_literal(char *p, char * buffer, size_t bufSize);
static void extract_unbounded_literal(char *p, char * buffer, size_t bufSize);

const char * visible[] = {
    "NOT_FOUND,      /* Token not found */",
    "ROOT,           /* $ */",
    "CUR_NODE,       /* @ */",
    "WILD_CARD,      /* * */",
    "DEEP_SCAN,      /* .. */",
    "NODE,           /* .child, ['child'] */",
    "EXPR_END,       /* ] */",
    "SLICE,          /* : */",
    "CHILD_SEP,      /* , */",
    "EXPR_START,     /* ? */",
    "STR,            /* \"a string\" 'a string' */",
    "EQ,             /* == */",
    "NEQ,            /* != */",
    "LT,             /* < */",
    "LTE,            /* <= */",
    "GT,             /* > */",
    "GTE,            /* >= */",
    "RGXP,           /* =~ */",
    "IN,             /* in */",
    "NIN,            /* nin */",
    "SIZE,           /*   */",
    "EMPTY,          /* empty */",
    "PAREN_OPEN,     /* ( */",
    "PAREN_CLOSE,    /* ) */",
    "LITERAL         /* \"some string\" 'some string' */",
    "FILTER_START    /* [ */"
};

token scan(char ** p, char * buffer, size_t bufSize) {

    token found_token = NOT_FOUND;

    while(**p != '\0' && found_token == NOT_FOUND) {

        switch(**p) {

            case '$':
                found_token = ROOT;
                break;
            case '.':
                if(*(*p+1) == '.') {
                    (*p)++;
                    found_token = DEEP_SCAN;
                } else {
                    found_token = NODE;
                }

                (*p)++;

                extract_unbounded_literal(*p, buffer, bufSize);

                *p += strlen(buffer) - 1;

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
                        found_token = NODE;
                        break;
                    case '"':
                        extract_quoted_literal(*p, buffer, bufSize);
                        *p += strlen(buffer) + 2;

                        for (; **p != '\0' && **p == ' '; (*p)++);

                        if(**p != ']') {
                            //Lexing error
                        }
                        found_token = NODE;
                        break;
                    case '?':
                        found_token = EXPR_START;
                        break;
                    default:
                        /* Pick up start in next iteration, maybe simplify */
                        (*p)--;
                        found_token = FILTER_START;
                        break;
                }
                break;
            case ']':
                found_token = EXPR_END;
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
                *p += strlen(buffer) + 1;
                found_token = LITERAL;
                break;
            case '"':
                extract_quoted_literal(*p, buffer, bufSize);
                *p += strlen(buffer) + 1;
                found_token = LITERAL;
                break;
            case '*':
                found_token = WILD_CARD;
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
