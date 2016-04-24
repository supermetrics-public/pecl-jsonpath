#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "expr.h"
#include "stack.h"

void output_postifx_expr(expr * expr, int count);

const char * const visible[] = {
    "=\0",
    "!=\0",
    "<\0",
    "<=\0",
    ">\0",
    ">=\0",
    "ISSET\0",
    "||\0",
    "&&\0",
    "(\0",
    ")\0",
    "<\0"
};

token_type get_token_type(token token) {

    switch(token) {
        case EQ:
        case NE:
        case LT:
        case LTE:
        case GT:
        case GTE:
        case OR:
        case AND:
               return TYPE_OPERATOR;
        case PAREN_LEFT:
        case PAREN_RIGHT:
               return TYPE_PAREN;
        case LITERAL:
               return TYPE_OPERAND;
    }
}

void testOne() {

    expr expr_in[] = {
        {
            LITERAL,
            "OP1"
        },
        {
            AND
        },
        {
            LITERAL,
           "OP2"
        },
        {
            OR
        },
        {
            LITERAL,
            "OP3"
        },
        {
            AND
        },
        {
            LITERAL,
           "OP4"
        },
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);
    printf("Expected: OP1 OP2 && OP3 || OP4 &&\n");
    printf("Actual:   ");
    output_postifx_expr(expr_out, out_count);
    printf("\n");
}

void testTwo() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "OP1"
        },
        {
            AND
        },
        {
            LITERAL,
           "OP2"
        },
        {
            PAREN_RIGHT
        },
        {
            OR
        },
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "OP3"
        },
        {
            AND
        },
        {
            LITERAL,
           "OP4"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);
    printf("Expected: OP1 OP2 && OP3 OP4 && ||\n");
    printf("Actual:   ");
    output_postifx_expr(expr_out, out_count);
    printf("\n");
}

void testThree() {

    expr expr_in[] = {
        {
            LITERAL,
            "OP1"
        },
        {
            AND
        },
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "OP2"
        },
        {
            OR
        },
        {
            LITERAL,
           "OP3"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);
    printf("Expected: OP1 OP2 OP3 || &&\n");
    printf("Actual:   ");
    output_postifx_expr(expr_out, out_count);
    printf("\n");
}

bool testFour() {

    callback_instance cb;

    strcpy(cb.lh.value, "10\0");
    strcpy(cb.rh.value, "30\0");

    cb.compare_fn = compare_lt;

    bool result;

    result = cb.compare_fn(&cb.lh, &cb.rh);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(result) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }

    return result;
}

bool testFive() {

    callback_instance cb;

    strcpy(cb.lh.value, "30\0");
    strcpy(cb.rh.value, "10\0");

    cb.compare_fn = compare_lt;

    bool result;

    result = cb.compare_fn(&cb.lh, &cb.rh);

    printf("Expected: FALSE\n");
    printf("Actual:   ");

    if(result) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }

    return result;
}

bool testSix() {

    callback_instance cb;

    strcpy(cb.lh.value, "10\0");
    strcpy(cb.rh.value, "30\0");

    cb.compare_fn = compare_gt;

    bool result;

    result = cb.compare_fn(&cb.lh, &cb.rh);

    printf("Expected: FALSE\n");
    printf("Actual:   ");

    if(result) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }

    return result;
}

bool testSeven() {

    callback_instance cb;

    strcpy(cb.lh.value, "30\0");
    strcpy(cb.rh.value, "10\0");

    cb.compare_fn = compare_gt;

    bool result;

    result = cb.compare_fn(&cb.lh, &cb.rh);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(result) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }

    return result;
}

int main() {

    printf("**** Test 1 ****\n");
    testOne();
    printf("**** Test 2 ****\n");
    testTwo();
    printf("**** Test 3 ****\n");
    testThree();
    printf("**** Test 4 ****\n");
    testFour();
    printf("**** Test 5 ****\n");
    testFive();
    printf("**** Test 6 ****\n");
    testSix();
    printf("**** Test 7 ****\n");
    testSeven();

    return 0;
}

bool compare_lt(expr * lh, expr * rh) {
    return atoi((*lh).value) < atoi((*rh).value);
}

bool compare_gt(expr * lh, expr * rh) {
    return atoi((*lh).value) > atoi((*rh).value);
}

void output_postifx_expr(expr * expr, int count) {

    int i;

    for(i = 0; i < count; i++) {
        switch(get_token_type(expr[i].type)) {
            case TYPE_OPERATOR:
                printf("%s ", visible[expr[i].type]);
                break;
            case TYPE_OPERAND:
            case TYPE_PAREN:
                printf("%s ", expr[i].value);
                break;
        }
    }
}

// See http://csis.pace.edu/~wolf/CS122/infix-postfix.htm
void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int * out_count) {

    Stack S;
    Stack_Init(&S);
    expr * expr_tmp;

    *out_count = 0;

    int i;

    for(i = 0; i < in_count; i++) {
        switch(get_token_type(expr_in[i].type)) {
            case TYPE_OPERAND:
                expr_out[(*out_count)++] = expr_in[i];
                break;
            case TYPE_OPERATOR:
                if(!S.size || (*Stack_Top(&S)).type == PAREN_LEFT) {
                    Stack_Push(&S, &expr_in[i]);
                } else {
                    expr_out[(*out_count)++] = *Stack_Top(&S);
                    Stack_Pop(&S);
                    Stack_Push(&S, &expr_in[i]);
                }
                break;
            case TYPE_PAREN:
                if(expr_in[i].type == PAREN_LEFT) {
                    Stack_Push(&S, &expr_in[i]);
                } else {
                    while(S.size) {
                        expr_tmp = Stack_Top(&S);
                        Stack_Pop(&S);
                        if((*expr_tmp).type == PAREN_LEFT) {
                            break;
                        }
                        expr_out[(*out_count)++] = *expr_tmp;
                    }
                }
                break;
        }
    }

    for(
        ;
        S.size;
        expr_tmp = Stack_Top(&S),
        expr_out[(*out_count)++] = *expr_tmp,
        Stack_Pop(&S)
    );

}