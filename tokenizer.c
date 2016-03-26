#include "tokenizer.h"
#include <stdbool.h>

int extract_bracket_contents(char * start, char *buffer);
void tokenize_bracket_contents(char * contents, struct token * tok);

bool tokenize(char ** p, struct token * tok) {

    char buffer[100];

    switch(**p) {

        case '$':
            tok->type = ROOT;
            break;
        case '.':
            if(*(*p+1) == '.') {
                (*p)++;
                tok->type = DEEP_SCAN;
            } else if(*(*p+1) == '*') {
                (*p)++;
                tok->type = WILD_CARD;
                break;
            } else {
                tok->type = CHILD_KEY;
                tok->prop.child.type = SINGLE_KEY;
            }

            strcpy(tok->prop.child.val, ""); //TODO Needed?

            while(*(*p+1) != '\0' && *(*p+1) != '.' && *(*p+1) != '[') {
                (*p)++;
                strncat(tok->prop.child.val, *p, 1);
            }

            (*p)++;
            if(**p == '[') {

                memset(buffer,0,strlen(buffer));

                if(!extract_bracket_contents(*p, buffer)) {
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

int extract_bracket_contents(char * start, char *buffer) {

    if(*start != '[') {
        return 0;
    }

    int x = 0;
    char * p;
    p = start;

    while(*p != '\0') {
        if(*p == '[') {
            --x;
        }

        if(*p == ']') {
            ++x;
            if(x == 0) {
                start++;
                strncpy(buffer, start, p - start);
                strcat(buffer, "\0");
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

    what = strpbrk(contents, ":*,");

    if(what != NULL) {
        switch(*what) {
            case ':':
                what = strtok(contents, ":");
                tok->prop.child.indexes[0] = atoi(what); //TODO error checking
                what = strtok(NULL, ":");
                tok->prop.child.indexes[1] = atoi(what); //TODO error checking
                tok->prop.child.index_count = 2;
                tok->prop.child.type = RANGE;
                break;
            case '*':
                printf("ANYaaaa");
                tok->prop.child.type = ANY;
                break;
            case ',':
                tok->prop.child.type = INDEX;

                count = 0;
                what = strtok(contents, ",");
                tok->prop.child.index_count = 0;

                while(what != NULL) {
                    tok->prop.child.indexes[count++] = atoi(what); //TODO error checking
                    tok->prop.child.index_count++;
                    what = strtok(NULL, ",");
                }
                break;
        }
    } else {
        tok->prop.child.index_count = 1;
        tok->prop.child.indexes[0] = atoi(contents); //TODO error checking
        tok->prop.child.type = INDEX;
    }
}
