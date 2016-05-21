#include "lexer.h"
#include <string.h>
#include <stdbool.h>

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

void print_test_behavior(
    char * description,
    char ** p,
    char * buffer,
    size_t bufSize
);

bool evaluate_test(
    token expected_token,
    char * expected_value,
    char * expected_remaining,
    token actual_token,
    char * actual_value,
    char * actual_remaining
);

bool test(
    char * description,
    char * input_str,
    token expected_token,
    char * expected_value,
    char * expected_remaining
);


int main() {

    int failures = 0, successes = 0;

    test(
        "return no results found for empty path",
        "", NOT_FOUND, "", ""
    ) ? successes++: failures++;

    test(
        "parse root node operator",
        "$.nodename", ROOT, "", ".nodename"
    ) ? successes++: failures++;

    test(
        "parse current node operator",
        "@.nodename", CUR_NODE, "", ".nodename"
    ) ? successes++: failures++;

    test(
        "parse node operator in dot notation",
        ".nodename.nextnode", NODE, "nodename", ".nextnode"
    ) ? successes++: failures++;

    test(
        "parse node operator in bracket notation and single quotes",
        "['nodename']['nextnode']", NODE, "nodename", "['nextnode']"
    ) ? successes++: failures++;

    test(
        "parse node operator in bracket notation and double quotes",
        "[\"nodename\"][\"nextnode\"]", NODE, "nodename", "[\"nextnode\"]"
    ) ? successes++: failures++;

    test(
        "parse deep scan (recursive) operator in dot notation",
        "..nodename.nodename", DEEP_SCAN, "nodename", ".nodename"
    ) ? successes++: failures++;

    test(
        "parse deep scan (recursive) operator in bracket notation",
        "..['nodename']..['nodename']", DEEP_SCAN, "nodename", "..['nodename']"
    ) ? successes++: failures++;

    test(
        "parse wild card operator",
        "*.nodeName", WILD_CARD, "", ".nodeName"
    ) ? successes++: failures++;

    test(
        "parse an expression terminator operator",
        "].nodename", EXPR_END, "", ".nodename"
    ) ? successes++: failures++;

    test(
        "parse an expression slice operator",
        ":.nodename", SLICE, "", ".nodename"
    ) ? successes++: failures++;

    test(
        "parse an expression child separator operator",
        ",.nodename", CHILD_SEP, "", ".nodename"
    ) ? successes++: failures++;

    test(
        "parse the start of an expression",
        "[?()]", EXPR_START, "", "()]"
    ) ? successes++: failures++;

    test(
        "parse a string literal in single quotes",
        "'here be an expression'==", LITERAL, "here be an expression", "=="
    ) ? successes++: failures++;

    test(
        "parse a string literal in single quotes with single quotes inside",
        "'here be 'an' expression'==", LITERAL, "here be 'an' expression", "=="
    ) ? successes++: failures++;

    test(
        "parse a string literal in single quotes with double quotes inside",
        "'here be \"an\" expression'==", LITERAL, "here be \"an\" expression", "=="
    ) ? successes++: failures++;

    test(
        "parse a string literal in double quotes",
        "\"here be an expression\"==", LITERAL, "here be an expression", "=="
    ) ? successes++: failures++;

    test(
        "parse a string literal in double quotes with single quotes inside",
        "\"here be 'an' expression\"==", LITERAL, "here be 'an' expression", "=="
    ) ? successes++: failures++;

    test(
        "parse a string literal in double quotes with double quotes inside",
        "\"here be \\\"an\\\" expression\"==", LITERAL, "here be \\\"an\\\" expression", "=="
    ) ? successes++: failures++;

    test(
        "parse an equals operator",
        "== .nodename", EQ, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse a not equals operator",
        "!= .nodename", NEQ, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse a less-than operator",
        "< .nodename", LT, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse a less-than-or-equals-to operator",
        "<= .nodename", LTE, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse a less-than operator",
        "> .nodename", GT, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse a greater-than-or-equals-to operator",
        ">= .nodename", GTE, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse a regex operator",
        "=~ .nodename", RGXP, "", " .nodename"
    ) ? successes++: failures++;

    test(
        "parse an the filter start operator for array slice or index selection",
        "[100:200]", FILTER_START, "", "100:200]"
    ) ? successes++: failures++;

    test(
        "parse an expression paren open operator",
        "(.nodename)", PAREN_OPEN, "", ".nodename)"
    ) ? successes++: failures++;

    test(
        "parse an expression paren open operator",
        ") && .nodename", PAREN_CLOSE, "", " && .nodename"
    ) ? successes++: failures++;


    printf("\n--------------------\n\n");
    printf("%d test(s) executed\n", successes + failures);
    printf("Success:\t%d\n", successes);
    printf("Failures:\t%d\n\n", failures);

    return 0;
}



void print_test_behavior(
    char * description,
    char ** p,
    char * buffer,
    size_t bufSize
) {
    printf("\n--------------------\n\n");
    printf("scan()\n\n");
    printf("With parameters:\n");
    printf(
        "\t- %s%s\n\t- %s%s\n\t- %d\n\n",
        *p, strlen(*p) ? "" : "(Empty)", buffer, strlen(buffer) ? "" : "(Empty)", (int)bufSize
    );
    printf("Should:\n");
    printf("\t%s\n\n", description);
}

bool evaluate_test(
    token expected_token,
    char * expected_value,
    char * expected_remaining,
    token actual_token,
    char * actual_value,
    char * actual_remaining
) {
    printf(ANSI_COLOR_BLUE "Expected:\n" ANSI_COLOR_RESET);
    printf(
        "\ttoken\t%s\n"
        "\tvalue\t'%s%s'\n"
        "\tremain\t'%s%s'\n\n",
        visible[expected_token],
        expected_value, strlen(expected_value) ? "" : "(Empty)",
        expected_remaining, strlen(expected_remaining) ? "" : "(Empty)"
    );
    printf(ANSI_COLOR_BLUE "Actual:\n" ANSI_COLOR_RESET);
    printf(
        "\ttoken\t%s\n"
        "\tvalue\t'%s%s'\n"
        "\tremain\t'%s%s'\n\n",
        visible[actual_token],
        actual_value, strlen(actual_value) ? "" : "(Empty)",
        actual_remaining, strlen(actual_remaining) ? "" : "(Empty)"
    );
    printf("Result:\n");
    if(
        strcmp(expected_value, actual_value) == 0
        && expected_token == actual_token
        && strcmp(expected_remaining, actual_remaining) == 0
    ) {
        printf(ANSI_COLOR_GREEN "\tSuccess\n" ANSI_COLOR_RESET);
        return true;
    } else {
        printf(ANSI_COLOR_RED "\tFailure\n" ANSI_COLOR_RESET);
        return false;
    }
}

bool test(
    char * description,
    char * input_str,
    token expected_token,
    char * expected_value,
    char * expected_remaining
) {

    token actual_token;
    char buffer[1000];
    buffer[0] = '\0';

    print_test_behavior(
        description,
        &input_str,
        buffer,
        sizeof(buffer)
    );

    actual_token = scan(&input_str, buffer, sizeof(buffer));

    return evaluate_test(
        expected_token,
        expected_value,
        expected_remaining,
        actual_token,
        buffer,
        input_str
    );
}