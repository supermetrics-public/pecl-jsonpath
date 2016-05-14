#include "jsonpathtokenizer.h"
#include <stdbool.h>

int extract_bracket_contents(char * start, char *buffer);
void tokenize_bracket_contents(char * contents, struct token * tok);
void tokenize_filter_expression(char * contents, struct token * tok);

typedef enum {
    SQUARE_BRACKETS,
    PARENTHESES,
} balanced_brackets_t;

typedef enum {
    DOUBLE_QUOTES,
    SINGLE_QUOTES
} balanced_quotes_t;

bool tokenize(char ** p, struct token * tok) {

    char buffer[250];

    switch(**p) {

        case '$':
            tok->type = ROOT;
            break;
        case '[':
        case '.':

            if(*(*p+1) == '.') {
                (*p)++;
                tok->type = DEEP_SCAN;
                tok->prop.type = SINGLE_KEY;
                tok->prop.index_count = 0;
            } else if(*(*p+1) == '*') {
                (*p)++;
                tok->type = WILD_CARD;
                break;
            } else {
                tok->type = CHILD_KEY;
                tok->prop.type = SINGLE_KEY;
            }

            /*
             * Extract dot-notated or bracket-notated property name
             */
            if(*(*p+1) == '[') {
                (*p)++;
            }

            if(*(*p+1) == '\'') {
                (*p)++;
            }

            strcpy(tok->prop.val, ""); //TODO Needed?

            /*
             * Keep extracting until property selector ends
             */
            while(*(*p+1) != '\0' && *(*p+1) != '.' && *(*p+1) != '[' && *(*p+1) != ']' && *(*p+1) != '\'') {
                (*p)++;
                strncat(tok->prop.val, *p, 1);
            }

            if(*(*p+1) == '\'') {
                (*p)++;
            }

            if(*(*p+1) == ']') {
                (*p)++;
            }

            /*
             * Is next token an array index operator?
             */
            if(*(*p+1) == '[' && *(*p+2 ) != '\'') {
                (*p)++;
            }

            /*
             * Parse array index operator
             */
            if(**p == '[') {

                memset(buffer,0,strlen(buffer));

                if(!extract_balanced_element_contents(*p, buffer, SQUARE_BRACKETS)) {
                    printf("Unable to extract contents");
                }

                (*p) += strlen(buffer) + 1;

                tokenize_bracket_contents(buffer, tok);
            }
            break;
        case '\0':
        default:
            return false;
    }

    (*p)++;

    return true;
}

int extract_balanced_element_contents(char * start, char *buffer, balanced_brackets_t b_type) {

    char open;
    char close;

    switch(b_type) {
        case SQUARE_BRACKETS:
            open = '[';
            close = ']';
            break;
        case PARENTHESES:
            open = '(';
            close = ')';
            break;
    }

    if(*start != open) {
        return 0;
    }

    int x = 0;
    char * p;
    p = start;

    while(*p != '\0') {
        if(*p == open) {
            --x;
        }

        if(*p == close) {
            ++x;
            if(x == 0) {
                start++;
                strncpy(buffer, start, p - start);
                buffer[(p - start)] = '\0';
                return 1;
            }
        }

        ++p;
    }

    return 0;
}

int extract_quoted_contents(char * start, char *buffer, balanced_quotes_t b_type) {

    const char * quote;

    switch(b_type) {
        case DOUBLE_QUOTES:
            quote = "\"";
            break;
        case SINGLE_QUOTES:
            quote = "'";
            break;
    }

    if(*start != *quote) {
        return 0;
    }

    start++;

    char * needle;

    needle = strstr(start, quote);

    if(*needle == *quote) {
        strncpy(buffer, start, needle - start);
        buffer[(needle - start)] = '\0';
        return 1;
    }

    return 0;
}

void tokenize_bracket_contents(char * contents, struct token * tok)
{
    char * what;
    int count = 0;

    what = strpbrk(contents, ":*,?");

    if(what != NULL) {
        switch(*what) {
            case ':':
                what = strtok(contents, ":");
                tok->prop.indexes[0] = atoi(what); //TODO error checking
                what = strtok(NULL, ":");
                tok->prop.indexes[1] = atoi(what); //TODO error checking
                tok->prop.index_count = 2;
                tok->prop.type = RANGE;
                break;
            case '*':
                tok->prop.type = ANY;
                break;
            case ',':
                tok->prop.type = INDEX;

                count = 0;
                what = strtok(contents, ",");
                tok->prop.index_count = 0;

                while(what != NULL) {
                    tok->prop.indexes[count++] = atoi(what); //TODO error checking
                    tok->prop.index_count++;
                    what = strtok(NULL, ",");
                }
                break;
            case '?':
                tok->prop.type = FILTER;
                contents++;
                tokenize_filter_expression(contents, tok);
                break;

        }
    } else {
        tok->prop.index_count = 1;
        tok->prop.indexes[0] = atoi(contents); //TODO error checking
        tok->prop.type = INDEX;
    }
}

/*
    Break down node hierarchy string into array
*/
void tok_node_name(expr * node, char * cur)
{
    int x = 0, cpy_len = 0;
    char * end, * start;

    end = strpbrk(cur, " )<>");

    node->label_count = 0;

    if(end == NULL) {
        fprintf(stderr, "Error: Unable to find end of node name\n");
        return;
    }

    for(; cur < end; cur++) {

        if(x == MAX_NODE_DEPTH) {
            printf("Error: Exceeded max node depth of %d\n", MAX_NODE_DEPTH);
            return;
        }

        /* Find the start of an expr segment */
        if(*cur == '.') {
            start = (cur + 1);
        } else {
            /*
                If this character is the last in the current segment, copy
                the segment name over
            */
            if(*(cur + 1) == '.' || (cur + 1) == end) {

                cpy_len = cur - start + 1;

                if(cpy_len > MAX_NODE_NAME_LEN - 1) {
                    printf("Error: Exceeded max node name length of %d\n", MAX_NODE_NAME_LEN - 1);
                    return;
                }

                strncpy(node->label[x], start, cpy_len);
                node->label[x][cpy_len] = '\0';
                node->label_count++;

                x++;
            }
        }
    }

    int i;

    for(i = 0; i < node->label_count; i++) {
        printf("Label: %s\n", node->label[i]);
    }
    
    printf("Total Labels: %d\n", node->label_count);
}

void tokenize_filter_expression(char * contents, struct token * tok)
{
    char * p;
    char buffer[250];
    char buffer2[250];
    char * what;
    int i = 0;
    expr expr_list[100];

    if(!extract_balanced_element_contents(contents, buffer, PARENTHESES)) {
        printf("Unable to extract contents ()");
    }

    p = buffer;

    while(*p != '\0') {

        memset(buffer2,0,strlen(buffer2));

        switch(*p) {

            case '@':
                p++;

                tok_node_name(&expr_list[i], p);

                expr_list[i].type = NODE_NAME;
                i++;
                break;
            case '\'':
                if(!extract_quoted_contents(p, buffer2, SINGLE_QUOTES)) {
                    printf("Unable to extract contents ''");
                } else {
                    p += strlen(buffer2) + 1;

                    strcpy(expr_list[i].value, buffer2);
                    expr_list[i].type = LITERAL;
                    i++;
                }
                break;
            case '"':
                if(!extract_quoted_contents(p, buffer2, DOUBLE_QUOTES)) {
                    printf("Unable to extract contents ''");
                } else {
                    p += strlen(buffer2) + 1;

                    strcpy(expr_list[i].value, buffer2);
                    expr_list[i].type = LITERAL;
                    i++;
                }
                break;
            case '<':
                if(*(p+1) == '=') {
                    expr_list[i].type = LTE;
                    p++;
                } else {
                    expr_list[i].type = LT;
                }
                i++;
                break;
            case '>':
                if(*(p+1) == '=') {
                    expr_list[i].type = GTE;
                    p++;
                } else {
                    expr_list[i].type = GT;
                }
                i++;
                break;
            case '!':
                break;
            case '=':
                if(*(p + 1) != '=') {
                    printf("There is an error");
                }
                p++;
                expr_list[i].type = EQ;
                i++;
                break;
            case '|':
                if(*(p + 1) != '|') {
                    printf("There is an error");
                }
                p++;
                expr_list[i].type = OR;
                i++;
                break;
            case '&':
                if(*(p + 1) != '&') {
                    printf("There is an error");
                }
                p++;
                expr_list[i].type = AND;
                i++;
                break;
            case ')':
                break;
            case '$':
                break;
            case '/':
                break;
            case '.':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':

                what = strpbrk(p, " )<>");

                //TODO GETTING END OF STRING
                if (what) {
                    strncpy(buffer2, p, what - p);
                    buffer2[(what - p)] = '\0';
                } else {
                    strcpy(buffer2, p);
                }

                expr_list[i].type = LITERAL;
                strcpy(expr_list[i].value, buffer2);

                p += strlen(buffer2) + 1;
                i++;
                break;
            default:
                break;
        }

        p++;
    }
//printf("The count is %d\n", i);
//printf("The val is %s\n", expr_list[0].value);
//printf("The type is %d\n", expr_list[0].type);
//output_postifx_expr(expr_list, i);
    printf("CONVERT TO POSTFIX\n");
    convert_to_postfix(expr_list, i, tok->prop.expr_list, &tok->prop.expr_count);
//printf("The count is %d\n", tok->prop.expr_count);
}

/** START Code imported from expr.c **/

void Stack_Init(Stack *S)
{
    S->size = 0;
}

expr * Stack_Top(Stack *S)
{
    if (S->size == 0) {
        fprintf(stderr, "Error: stack empty\n");
        return NULL;
    }

    return S->data[S->size-1];
}

void Stack_Push(Stack *S, expr * expr)
{
    if (S->size < STACK_MAX)
        S->data[S->size++] = expr;
    else
        fprintf(stderr, "Error: stack full\n");
}

void Stack_Pop(Stack *S)
{
    if (S->size == 0)
        fprintf(stderr, "Error: stack empty\n");
    else
        S->size--;
}

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
        case NODE_NAME:
        case BOOL:
               return TYPE_OPERAND;
    }
}

bool evaluate_postfix_expression(expr * expression_original, int count) {


    expr expression[100] = {0};

    memcpy(expression, expression_original, sizeof(expr) * count);

//    memcpy ( &expression, &expression_original, sizeof(expr)* count);


    Stack S;
    Stack_Init(&S);
    expr * expr_lh;
    expr * expr_rh;
    bool temp_res;
    int i;

    for(i = 0; i < count; i++) {

        switch(get_token_type(expression[i].type)) {
            case TYPE_OPERATOR:
                expr_rh = Stack_Top(&S);
                Stack_Pop(&S);

                expr_lh = Stack_Top(&S);

                temp_res = exec_cb_by_token(expression[i].type)(expr_lh, expr_rh);

                (*expr_lh).type = BOOL;
                (*expr_lh).value_bool = temp_res;
                break;
            case TYPE_OPERAND:
                Stack_Push(&S, &expression[i]);
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

/** END Code imported from expr.c **/