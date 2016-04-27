#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "expr.h"
#include "stack.h"

void output_postifx_expr(expr * expr, int count);
bool compare_lt(expr * lh, expr * rh);
bool compare_gt(expr * lh, expr * rh);
bool compare_and(expr * lh, expr * rh);
bool compare_or(expr * lh, expr * rh);
bool compare_eq(expr * lh, expr * rh);


const char * const visible[] = {
    "==\0",
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

void test1() {

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

void test2() {

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

void test3() {

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

bool test4() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            PAREN_RIGHT
        },
        {
            AND
        },
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);
    printf("Expected: 10 30 < 30 10 > &&\n");
    printf("Actual:   ");
    output_postifx_expr(expr_out, out_count);
    printf("\n");

}

bool test4a() {

    expr expr_in[] = {
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            AND
        },
        {
            LITERAL,
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);
    printf("Expected: 10 30 < 30 10 > &&\n");
    printf("Actual:   ");
    output_postifx_expr(expr_out, out_count);
    printf("\n");

}

bool test4b() {

    expr expr_in[] = {
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            AND
        },
        {
            LITERAL,
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            AND
        },
        {
            LITERAL,
            "10"
        },
        {
            EQ
        },
        {
            LITERAL,
            "10"
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);
    printf("Expected: 10 30 < 30 10 > && 10 10 == &&\n");
    printf("Actual:   ");
    output_postifx_expr(expr_out, out_count);
    printf("\n");

}

bool test4c() {

    expr expr_in[] = {
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            AND
        },
        {
            LITERAL,
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            AND
        },
        {
            LITERAL,
            "10"
        },
        {
            EQ
        },
        {
            LITERAL,
            "10"
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test4d() {

    expr expr_in[] = {
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            AND
        },
        {
            LITERAL,
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            AND
        },
        {
            LITERAL,
            "10"
        },
        {
            EQ
        },
        {
            LITERAL,
            "20"
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: FALSE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test5() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            PAREN_RIGHT
        },
        {
            AND
        },
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test6() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            PAREN_RIGHT
        },
        {
            AND
        },
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "10"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: FALSE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test7() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "40"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
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
            "30"
        },
        {
            GT
        },
        {
            LITERAL,
            "10"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test8() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
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
            "10"
        },
        {
            GT
        },
        {
            LITERAL,
            "30"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }

}

bool test9() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "50"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
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
            "10"
        },
        {
            GT
        },
        {
            LITERAL,
            "30"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: FALSE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test10() {

    expr expr_in[] = {
        {
            PAREN_LEFT
        },
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
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
            "50"
        },
        {
            GT
        },
        {
            LITERAL,
            "30"
        },
        {
            PAREN_RIGHT
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test10a() {

    expr expr_in[] = {
        {
            LITERAL,
            "10"
        },
        {
            LT
        },
        {
            LITERAL,
            "30"
        },
        {
            OR
        },
        {
            LITERAL,
            "50"
        },
        {
            GT
        },
        {
            LITERAL,
            "30"
        }
    };

    expr expr_out[100];

    int in_count = sizeof(expr_in) / sizeof(expr_in[0]),
        out_count = 0;

    convert_to_postfix(expr_in, in_count, expr_out, &out_count);

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_out, out_count)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test11() {

    expr expr_in[] = {
        {
            LITERAL,
            "10"
        },
        {
            LITERAL,
            "30"
        },
        {
            LT
        }
    };

    printf("Expected: TRUE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_in, 3)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

bool test12() {

    expr expr_in[] = {
        {
            LITERAL,
            "30"
        },
        {
            LITERAL,
            "10"
        },
        {
            LT
        }
    };

    printf("Expected: FALSE\n");
    printf("Actual:   ");

    if(evaluate_postfix_expression(expr_in, 3)) {
        printf("TRUE\n");
    } else {
        printf("FALSE\n");
    }
}

int main() {

    printf("**** Test 1 ****\n");
    test1();
    printf("**** Test 2 ****\n");
    test2();
    printf("**** Test 3 ****\n");
    test3();
    printf("**** Test 4 ****\n");
    test4();
    printf("**** Test 4a ****\n");
    test4a();
    printf("**** Test 4b ****\n");
    test4b();
    printf("**** Test 4c ****\n");
    test4c();
    printf("**** Test 4d ****\n");
    test4d();
    printf("**** Test 5 ****\n");
    test5();
    printf("**** Test 6 ****\n");
    test6();
    printf("**** Test 7 ****\n");
    test7();
    printf("**** Test 8 ****\n");
    test8();
    printf("**** Test 9 ****\n");
    test9();
    printf("**** Test 10 ****\n");
    test10();
    printf("**** Test 10a ****\n");
    test10a();
    printf("**** Test 11 ****\n");
    test11();
    printf("**** Test 12 ****\n");
    test12();

    return 0;
}

bool compare_lt(expr * lh, expr * rh) {
    return atoi((*lh).value) < atoi((*rh).value);
}

bool compare_gt(expr * lh, expr * rh) {
    return atoi((*lh).value) > atoi((*rh).value);
}

bool compare_and(expr * lh, expr * rh) {
    return (*lh).value_bool && (*rh).value_bool;
}

bool compare_or(expr * lh, expr * rh) {
    return (*lh).value_bool || (*rh).value_bool;
}

bool compare_eq(expr * lh, expr * rh) {
    return atoi((*lh).value) == atoi((*rh).value);
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

bool evaluate_postfix_expression(expr * expression, int count) {

    Stack S;
    Stack_Init(&S);
    expr * expr_lh;
    expr * expr_rh;
    bool temp_res;
    int i;

    for(i = 0; i < count; i++) {
        switch(get_token_type(expression[i].type)) {
            case TYPE_OPERAND:
                Stack_Push(&S, &expression[i]);
                break;
            case TYPE_OPERATOR:
                expr_rh = Stack_Top(&S);
                Stack_Pop(&S);
                expr_lh = Stack_Top(&S);

                temp_res = exec_cb_by_token(expression[i].type)(expr_lh, expr_rh);

                (*expr_lh).type = BOOL;
                (*expr_lh).value_bool = temp_res;
                break;
        }
    }

    return (*expr_lh).value_bool;
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

                    expr_tmp = Stack_Top(&S);

                    //TODO compare macro or assign to var?
                    if(get_operator_precedence(expr_in[i].type) < get_operator_precedence((*expr_tmp).type)) {
                        Stack_Push(&S, &expr_in[i]);
                    } else if(get_operator_precedence(expr_in[i].type) > get_operator_precedence((*expr_tmp).type)) {
                        expr_out[(*out_count)++] = *Stack_Top(&S);
                        Stack_Pop(&S);
                        i--; //Try the incoming token again in the next loop iteration
                    } else {
                        expr_out[(*out_count)++] = *Stack_Top(&S);
                        Stack_Pop(&S);
                        Stack_Push(&S, &expr_in[i]);
                    }
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

compare_cb exec_cb_by_token(token token_type) {

    switch(token_type) {
        case EQ:
            return compare_eq;
        case NE:
            printf("Callback not supported yet");
           break;
        case LT:
            return compare_lt;
        case LTE:
            printf("Callback not supported yet");
           break;
        case GT:
            return compare_gt;
        case GTE:
            printf("Callback not supported yet");
           break;
        case ISSET:
            printf("Callback not supported yet");
           break;
        case OR:
            return compare_or;
           break;
        case AND:
            return compare_and;
        case PAREN_LEFT:
        case PAREN_RIGHT:
        case LITERAL:
        case BOOL:
        default:
            printf("Error, no callback for token");
            break;
    }
}

//TODO: Distinguish between operator and token?
int get_operator_precedence(token token_type) {

    switch(token_type) {
        case LT:
            return 0;
        case LTE:
            return 0;
           break;
        case GT:
            return 0;
        case GTE:
            return 0;
        case NE:
            return 1;
        case EQ:
            return 1;
        case AND:
            return 2;
        case OR:
            return 2;
        case ISSET:
        case PAREN_LEFT:
        case PAREN_RIGHT:
        case LITERAL:
        case BOOL:
        default:
            printf("Error, no callback for token");
            break;
    }
}