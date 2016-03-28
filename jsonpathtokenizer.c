#include "jsonpathtokenizer.h"
#include <stdbool.h>

int extract_bracket_contents(char * start, char *buffer);
void tokenize_bracket_contents(char * contents, struct token * tok);
void tokenize_filter_string(char * contents, struct token * tok);

typedef enum {
    SQUARE_BRACKETS,
    PARENTHESES
} balanced_brackets_t;

bool tokenize(char ** p, struct token * tok) {

    char buffer[250];

    switch(**p) {

        case '$':
            tok->type = ROOT;
            break;
        case '.':
            if(*(*p+1) == '.') {
                (*p)++;
                tok->type = DEEP_SCAN;
                tok->prop.type = SINGLE_KEY;
                tok->prop.index_count = 0;
            } else if(*(*p+1) == '*') {
                (*p)++;
                tok->type = WILD_CARD;
                break;
            } else {
                tok->type = CHILD_KEY;
                tok->prop.type = SINGLE_KEY;
            }

            strcpy(tok->prop.val, ""); //TODO Needed?

            while(*(*p+1) != '\0' && *(*p+1) != '.' && *(*p+1) != '[') {
                (*p)++;
                strncat(tok->prop.val, *p, 1);
            }

            if(*(*p+1) == '[') {
                (*p)++;
            }

            if(**p == '[') {

                memset(buffer,0,strlen(buffer));

                if(!extract_balanced_bracket_contents(*p, buffer, SQUARE_BRACKETS)) {
                    printf("Unable to extract contents");
                }

                (*p) += strlen(buffer) + 1;

                tokenize_bracket_contents(buffer, tok);
            }
            break;
        case '\0':
        default:
            return false;
    }

    (*p)++;

    return true;
}

int extract_balanced_bracket_contents(char * start, char *buffer, balanced_brackets_t b_type) {

    char open;
    char close;

    switch(b_type) {
        case SQUARE_BRACKETS:
            open = '[';
            close = ']';
            break;
        case PARENTHESES:
            open = '(';
            close = ')';
            break;
    }

    if(*start != open) {
        return 0;
    }

    int x = 0;
    char * p;
    p = start;

    while(*p != '\0') {
        if(*p == open) {
            --x;
        }

        if(*p == close) {
            ++x;
            if(x == 0) {
                start++;
                strncpy(buffer, start, p - start);
                buffer[(p - start)] = '\0';
                return 1;
            }
        }

        ++p;
    }

    return 0;
}

void tokenize_bracket_contents(char * contents, struct token * tok)
{
    char * what;
    int count = 0;

    what = strpbrk(contents, ":*,?");

    if(what != NULL) {
        switch(*what) {
            case ':':
                what = strtok(contents, ":");
                tok->prop.indexes[0] = atoi(what); //TODO error checking
                what = strtok(NULL, ":");
                tok->prop.indexes[1] = atoi(what); //TODO error checking
                tok->prop.index_count = 2;
                tok->prop.type = RANGE;
                break;
            case '*':
                tok->prop.type = ANY;
                break;
            case ',':
                tok->prop.type = INDEX;

                count = 0;
                what = strtok(contents, ",");
                tok->prop.index_count = 0;

                while(what != NULL) {
                    tok->prop.indexes[count++] = atoi(what); //TODO error checking
                    tok->prop.index_count++;
                    what = strtok(NULL, ",");
                }
                break;
            case '?':
                contents++;
                tokenize_filter_string(contents, tok);
                break;

        }
    } else {
        tok->prop.index_count = 1;
        tok->prop.indexes[0] = atoi(contents); //TODO error checking
        tok->prop.type = INDEX;
    }
}

void tokenize_filter_string(char * contents, struct token * tok)
{
    char * p;
    char buffer[250];

    if(!extract_balanced_bracket_contents(contents, buffer, PARENTHESES)) {
        printf("Unable to extract contents ()");
    }

    printf("Extracted string: %s\n", buffer);

    p = buffer;

}