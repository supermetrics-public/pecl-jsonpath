#include <stdio.h>
#include <stdbool.h>
#include <string.h> /* memset */

int main() {

    char string[] = "$.meowx.hiss.*.key";


    int bufSize = 100;
    char buffer[bufSize];

    char * curBuffer = buffer;

    char * p = string;

    bool buildingKeyName = false;
    bool insideSubExpression = true;

    while(*p != '\0') {

        switch(*p) {
            case '$':
                printf("WE STARTIN!!\n");
                break;
            case '.':
                if(buildingKeyName) {
                    buildingKeyName = false;
                    *curBuffer = '\0';
                    printf("We found a new key name: %s\n", buffer);
                    curBuffer = buffer;
                }
                if(*(p+1) != '\0') {
                    switch(*(p+1)) {
                        case '.':
                            printf("This is a deep scan!\n");
                            break;
                        case '*':
                            printf("This is a wildcard match\n");
                            p++;
                            break;
                        case '[':
                            printf("We're in some sub-expression\n");
                            insideSubExpression = true;
                            break;
                        default:
                            buildingKeyName = true;
                            printf("We're building a key name\n");
                            break;
                    }
                }
                break;
            default:
                if(buildingKeyName) {
//                    printf("Building\n");
                    *curBuffer = *p;
                    curBuffer++;
                    if(*(p+1) == '\0') {
                        buildingKeyName = false;
                        *curBuffer = '\0';
                        printf("We found a new key name: %s\n", buffer);
                        curBuffer = buffer;
                    }
                }
        }

        p++;
    }

    return 0;
}
