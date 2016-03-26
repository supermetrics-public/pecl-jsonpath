#include "tokenizer.h"

int main() {

    struct token token_struct;
    char * input_str = "$.key.key2[7,8,9].key2[2,5].key2[1]..kedey";
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