#include "tokenizer.h"

int main() {

    int list_size = 100;
    int total_tokens = -1;

    struct token token_struct;

    token_struct.type = CHILD_KEY;

    char * input_str = "$..lol[111:116].key[3]";

    char * save_ptr = input_str;

    struct token * token_ptr = &token_struct;

    while(tokenize(&save_ptr, token_ptr)) {

        switch(token_struct.type) {
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
                printf("Deep scan - %s ", token_struct.prop.val);
                switch(token_struct.prop.type) {
                    case RANGE:
                        printf(" Index From %d to %d", token_struct.prop.indexes[0], token_struct.prop.indexes[1]);
                        break;
                    case INDEX:
                        for(int x = 0; x < token_struct.prop.index_count; x++) {
                            printf(" Index %d ", token_struct.prop.indexes[x]);
                        }
                        break;
                    case ANY:
                    case SINGLE_KEY:
                        break;
                }
                printf("\n");
                break;
            case CHILD_KEY:
                printf("Child key");
                switch(token_struct.prop.type) {
                    case RANGE:
                        printf(" Index From %d to %d", token_struct.prop.indexes[0], token_struct.prop.indexes[1]);
                        break;
                    case INDEX:
                        for(int x = 0; x < token_struct.prop.index_count; x++) {
                            printf(" Index %d ", token_struct.prop.indexes[x]);
                        }
                        break;
                    case ANY:
                    case SINGLE_KEY:
                        break;
                }
                printf("\n");
                break;
        }
    }

    return 0;
}