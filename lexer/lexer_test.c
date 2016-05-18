#include "lexer.h"
#include <string.h>

int main() {

    char * input_str = "@.some_texst['some_more_text']<>>=<==~:,==!=[?(@.field == 'VALUE' )]";

    char * p = input_str;

    token scan_val;

    char buffer[1000];

    printf("Token stream: \n");

    while((scan_val = scan(&p, buffer, sizeof(buffer))) != -1) {

        printf("%s ", visible[scan_val]);

        if(scan_val == CHILD_NODE || scan_val == LITERAL) {
            printf("the value is `%s`", buffer);
        }

        printf("\n");
    }

    return 0;
}