
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    DEFAULT,
    ROOT,
    WILD_CARD,
    DEEP_SCAN,
    CHILD_KEY
} token_type;

typedef enum {
    RANGE,
    INDEX,
    ANY
} child_type;

struct token {
    token_type type;
    union {
        struct {
            char val[100];
            child_type type;
            int index_count;
            int indexes[100];
        } child;
    } prop;
};

int extract_bracket_contents(char * start, char *buffer);

void tokenize(char * input, struct token * tok, int * length) {

    int i = 0;

    char * p = input;

    char * last;
    char buffer[100];

    char * what;
    char * pch;
    int count = 0;

    while(*p != '\0') {

        switch(*p) {

            case '$':
                tok[i].type = ROOT;
                i++;
                break;
            case '.':
                if(*(p+1) == '.') {
                    p++;
                    tok[i].type = DEEP_SCAN;
                } else if(*(p+1) == '*') {
                    p++;
                    tok[i].type = WILD_CARD;
                    i++;
                    continue;
                } else {
                    tok[i].type = CHILD_KEY;
                    tok[i].prop.child.type = INDEX;
                }

                strcpy(tok[i].prop.child.val, "");

                while(*(p+1) != '\0' && *(p+1) != '.' && *(p+1) != '[') {
                    strncat(tok[i].prop.child.val, ++p, 1);
                }

                p++;
                if(*p == '[') {
                    memset(buffer,0,strlen(buffer));

                    if(!extract_bracket_contents(p, buffer)) {
                        printf("Unable to extract contents");
                    }

                    what = strpbrk(buffer, ":*,");

                    if(what != NULL) {
                        switch(*what) {
                            case ':':
                                what = strtok(buffer, ":");
                                tok[i].prop.child.indexes[0] = atoi(what); //TODO error checking
                                what = strtok(NULL, ":");
                                tok[i].prop.child.indexes[1] = atoi(what); //TODO error checking
                                tok[i].prop.child.index_count = 2;
                                tok[i].prop.child.type = RANGE;
                                break;
                            case '*':
                                printf("ANYaaaa");
                                tok[i].prop.child.type = ANY;
                                break;
                            case ',':
                                printf("INDEXaaa");
                                tok[i].prop.child.type = INDEX;

                                count = 0;
                                what = strtok(buffer, ",");
                                tok[i].prop.child.index_count = 0;

                                while(what != NULL) {
                                    tok[i].prop.child.indexes[count++] = atoi(what); //TODO error checking
                                    tok[i].prop.child.index_count++;
                                    what = strtok(NULL, ",");
                                }
                                break;
                        }
                    } else {
                        tok[i].prop.child.index_count = 1;
                        tok[i].prop.child.indexes[0] = atoi(buffer); //TODO error checking
                        tok[i].prop.child.type = INDEX;
                    }
                }
                i++;

                break;
        }

        p++;
    }

    *length = i;
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

int main() {

    int list_size = 100;
    int total_tokens = -1;

    struct token token_list[list_size];


    memset(token_list,0,sizeof(token_list));

    tokenize("$.*.lol[1].hahah[1:12].hahahah[1,34,28,32]", token_list, &total_tokens);

//TODO total tokens incorrect
    for(int i = 0; i < total_tokens; i++) {
        switch(token_list[i].type) {
            case DEFAULT:
                printf("def node\n");
                break;
            case ROOT:
                printf("Root node\n");
                break;
            case WILD_CARD:
                printf("Wild card\n");
                break;
            case DEEP_SCAN:
                printf("Deep scan - %s\n", token_list[i].prop.child.val);
                break;
            case CHILD_KEY:
                printf("Child key");
                switch(token_list[i].prop.child.type) {
                    case RANGE:
                        printf(" Index From %d to %d", token_list[i].prop.child.indexes[0], token_list[i].prop.child.indexes[1]);
                        break;
                    case INDEX:
                        for(int x = 0; x < token_list[i].prop.child.index_count; x++) {
                            printf(" Index %d ", token_list[i].prop.child.indexes[x]);
                        }
                        break;
                    case ANY:
                        break;
                }
                printf("\n");
                break;
        }
    }
//    struct token a;
//    a.type = ROOT;
//    strcpy(a.prop.child.val, "Some string");
//
//    printf("%s", a.prop.child.val);
    return 0;
}