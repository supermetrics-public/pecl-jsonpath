#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "jsonpathtokenizer.h"

void output_postifx_expr(expr * expr, int count);

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
