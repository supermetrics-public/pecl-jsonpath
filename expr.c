#include<stdio.h>
#include<string.h>
#include "expr.h"
#include "stack.h"

void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int * out_count);
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

/*
RULES
First of all we need two things first a stack and a  Postfix string.
Scan the input Infix string from left to right.
If the input is an operand add it to the Postfix String.
If the input is an operator then two cases one the stack is empty or if it is not.
If the stack is empty we add the operator directly to the stack.
If the stack is non empty we add then we compare the precedence of the operator on the top of the stack to that we just got.
If the operator on the top of the stack has more precedence than the one we just got then we remove it from the stack and add it to the Postfix String. We go on doing it until the operator precedence on the top of the stack is less than the operator we just got.(Or until the stack is empty). Then we add the operator on the stack.
If we get a left parenthesis is found we add it directly to the stack.
If we get a right parenthesis we repeatedly pop out operators from the stack until we get a left parenthesis and add it to the Postfix String. Then we remove the left parenthesis. The parenthesis are not used in the Postfix expressions.
*/

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
    printf("Actual: ");
    output_postifx_expr(expr_out, out_count);
}

int main() {

    testOne();

    return 0;
}

void output_postifx_expr(expr * expr, int count) {

    int i;

    /** OUTPUT **/
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

void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int * out_count) {

    Stack S;
    Stack_Init(&S);
    expr * expr_tmp;

    *out_count = 0;

    int i = 0;

    for(i = 0; i < in_count; i++) {
        switch(get_token_type(expr_in[i].type)) {
            case TYPE_OPERAND:
                printf("Parsing operand\n");
                expr_out[(*out_count)++] = expr_in[i];
                break;
            case TYPE_OPERATOR:
                if(!S.size || (*Stack_Top(&S)).type == PAREN_LEFT) {
                    printf("Parsing operator: Empty or left paren\n");
                    Stack_Push(&S, &expr_in[i]);
                } else {
                    printf("Parsing operator: Not empty\n");
                    expr_out[(*out_count)++] = *Stack_Top(&S);
                    Stack_Pop(&S);
                    Stack_Push(&S, &expr_in[i]);
                }
                break;
            case TYPE_PAREN:
                if(expr_in[i].type == PAREN_LEFT) {
                    printf("Parsing left\n");
                    Stack_Push(&S, &expr_in[i]);
                } else {
                    printf("Parsing right\n");
    //                    for(
    //                        ;
    //                        S.size && (*expr_tmp).type != PAREN_LEFT;
    //                        Stack_Pop(&S), expr_tmp = Stack_Top(&S)
    //                    ) {
    //                        expr_out[(*out_count)++] = *expr_tmp;
    //                    }

                    Stack_Pop(&S);
                }
                break;
        }
    }

    for(
        ;
        S.size;
        expr_tmp = Stack_Top(&S), expr_out[(*out_count)++] = *expr_tmp, Stack_Pop(&S)
    );
}