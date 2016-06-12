#include "parser.h"
#include "lexer.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/*
 Source:
 http://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


int main() {

    lex_token lex_tok[100];
    char lex_tok_values[100][100];
    int lex_tok_count = 0;

    char * input_str = "$.prop2[*]";
    char * p = input_str;

    char buffer[100];

    lex_token * ptr = lex_tok;

    while((*ptr = scan(&p, buffer, sizeof(buffer))) != LEX_NOT_FOUND) {

        switch(*ptr) {
            case LEX_NODE:
            case LEX_LITERAL:
                strcpy(lex_tok_values[lex_tok_count], buffer);
                break;
            default:
                lex_tok_values[lex_tok_count][0] = '\0';
                break;
        }

        ptr++;

        lex_tok_count++;
    }


    struct token tok[100];
    int tok_count = 0;
    int * int_ptr = &tok_count;
    build_parse_tree(
        lex_tok,
        lex_tok_values,
        lex_tok_count,
        tok,
        int_ptr
    );


    printf("Total tokens\t %d\n\n", *int_ptr);

//
    printf("First property %s\n", tok[1].prop.val);
    printf("Expr property 1 %s\n", tok[1].prop.expr_list[0].label[0]);
    printf("Expr property 2 %s\n", tok[1].prop.expr_list[0].label[1]);

    printf("First property %s\n", tok[2].prop.val);
    printf("Expr property 1 %s\n", tok[2].prop.expr_list[0].label[0]);
    printf("Expr property 2 %s\n", tok[2].prop.expr_list[0].label[1]);

    printf("First property %s\n", tok[3].prop.val);
    printf("Expr property 1 %s\n", tok[3].prop.indexes[0]);
    printf("Expr property 2 %s\n", tok[3].prop.expr_list[1]);
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

bool isset2(expr * lh, expr * rh) {
    return (*rh).value_bool;
}