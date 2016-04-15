#include "jsonpathtokenizer.h"

const char* exprType[] = {
    "Node Value",
    "Expression"
};

int main() {

    struct token token_struct;
    char * input_str = "$.key[?(@.property >= 'const string')]";
    char * save_ptr = input_str;
    struct token * token_ptr = &token_struct;

    int x;

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
                        for(x = 0; x < token_struct.prop.index_count; x++) {
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
                switch(token_struct.prop.type) {
                    case RANGE:
                        printf("Child key Index From %d to %d", token_struct.prop.indexes[0], token_struct.prop.indexes[1]);
                        break;
                    case INDEX:
                        for(x = 0; x < token_struct.prop.index_count; x++) {
                            printf("Child key Index %d ", token_struct.prop.indexes[x]);
                        }
                        break;
                    case ANY:
                        break;
                    case SINGLE_KEY:
                        printf("Child key Name `%s` ", token_struct.prop.val);
                        break;
                    case FILTER:
                        switch(token_struct.prop.expr.op) {
                            case EQ:
                                printf(
                                    "Expression: = | lhs: (%s) %s | rhs: (%s) %s",
                                    exprType[token_struct.prop.expr.lh_type],
                                    token_struct.prop.expr.lh_val,
                                    exprType[token_struct.prop.expr.rh_type],
                                    token_struct.prop.expr.rh_val
                                );
                                break;
                            case NE:
                                printf(
                                    "Expression: != | lhs: (%s) %s | rhs: (%s) %s",
                                    exprType[token_struct.prop.expr.lh_type],
                                    token_struct.prop.expr.lh_val,
                                    exprType[token_struct.prop.expr.rh_type],
                                    token_struct.prop.expr.rh_val
                                );
                                break;
                            case LT:
                                printf(
                                    "Expression: < | lhs: (%s) %s | rhs: (%s) %s",
                                    exprType[token_struct.prop.expr.lh_type],
                                    token_struct.prop.expr.lh_val,
                                    exprType[token_struct.prop.expr.rh_type],
                                    token_struct.prop.expr.rh_val
                                );
                                break;
                            case LTE:
                                printf(
                                    "Expression: <= | lhs: (%s) %s | rhs: (%s) %s",
                                    exprType[token_struct.prop.expr.lh_type],
                                    token_struct.prop.expr.lh_val,
                                    exprType[token_struct.prop.expr.rh_type],
                                    token_struct.prop.expr.rh_val
                                );
                                break;
                            case GT:
                                printf(
                                    "Expression: > | lhs: (%s) %s | rhs: (%s) %s",
                                    exprType[token_struct.prop.expr.lh_type],
                                    token_struct.prop.expr.lh_val,
                                    exprType[token_struct.prop.expr.rh_type],
                                    token_struct.prop.expr.rh_val
                                );
                                break;
                            case GTE:
                                printf(
                                    "Expression: >= | lhs: (%s) %s | rhs: (%s) %s",
                                    exprType[token_struct.prop.expr.lh_type],
                                    token_struct.prop.expr.lh_val,
                                    exprType[token_struct.prop.expr.rh_type],
                                    token_struct.prop.expr.rh_val
                                );
                                break;
                            case ISSET:
                                printf("Expression: isset | lhs: (%s) %s ", exprType[token_struct.prop.expr.lh_type], token_struct.prop.expr.lh_val);
                                break;
                        }
                        break;
                }
                printf("\n");
                break;
        }
    }

    return 0;
}