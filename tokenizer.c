
#include <stdio.h>
#include <string.h>

typedef enum {
    ROOT,
    WILD_CARD,
    DEEP_SCAN,
    CHILD_KEY
} token_type;

struct token {
    token_type type;
    union {
        struct {
            char val[100];
        } child;
    } prop;
};

void tokenize(char * input, struct token * tok, int length) {

    int i = 0;

    char * p = input;

    while(*p != '\0') {

        switch(*p) {

            case '$':
                tok[i++].type = ROOT;
                break;
            case '.':
                if(*(p+1) == '.') {
                    p++;
                    tok[++i].type = DEEP_SCAN;
                } else if(*(p+1) == '*') {
                    p++;
                    tok[++i].type = WILD_CARD;
                    break;
                } else {
                    tok[++i].type = CHILD_KEY;
                }

                strcpy(tok[i].prop.child.val, "");

                while(*(p+1) != '\0' && *(p+1) != '.' && *(p+1) != '[') {
                    strncat(tok[i].prop.child.val, ++p, 1);
                }

                break;
            default:
                break;
        }

        p++;
    }
}

int main() {

    int list_size = 100;

    struct token token_list[list_size];

    tokenize("$..lol..hah..meow.somekey.keyfosho.*", token_list, list_size);

    for(int i = 0; i < 8; i++) {
        switch(token_list[i].type) {
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
                printf("Child key - %s\n", token_list[i].prop.child.val);
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