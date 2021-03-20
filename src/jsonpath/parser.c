#include "lexer.h"
#include "parser.h"
#include "stack.h"
#include <stdio.h>
#include "safe_string.h"

bool is_unary(expr_op_type);

// Return node count (not including last token)

static int get_expression_node_count(
    lex_token * lex_tok,
    int pos,
    int lex_tok_count
) {

    int count = 0;

    while (pos < lex_tok_count) {

        if (lex_tok[pos] == LEX_EXPR_END) {
            return count;
        }

        count++;

        pos++;
    }

    return -1;
}

static bool tokenize_expression(
    lex_token * lex_tok,
    int *pos,
    int lex_tok_count,
    operator * tok,
    char lex_tok_values[][PARSE_BUF_LEN],
    parse_error * err
) {

    expr_operator expr_list[PARSE_BUF_LEN];
    int i = 0, x = 0;

    while (*pos < lex_tok_count) {

	switch (lex_tok[*pos]) {
        case LEX_EXPR_START:
            break;
	case LEX_PAREN_OPEN:
	    expr_list[i].type = EXPR_PAREN_LEFT;
            i++;;
            break;
        case LEX_PAREN_CLOSE:
			if (expr_list[i - 1].type == EXPR_NODE_NAME) {
				expr_list[i].type = EXPR_ISSET;
				i++;
	    	}
	    	expr_list[i].type = EXPR_PAREN_RIGHT;
            i++;
            break;	
        case LEX_EXPR_END:
            convert_to_postfix(expr_list, i, tok->expressions, &tok->expression_count);
            return true;
        case LEX_NODE:
            break;
	case LEX_CUR_NODE:
	    x = 0;
	    expr_list[i].label_count = 0;
	    while ((*pos + 1) < lex_tok_count && lex_tok[(*pos) + 1] == LEX_NODE) {
		(*pos)++;
                if (x == MAX_NODE_DEPTH) {
                    strncpy(err->msg, "Buffer size exceeded", sizeof(err->msg));
                    return false;
                }
                expr_list[i].label[x] = lex_tok_values[*pos];
		expr_list[i].label_count++;
		x++;
	    }
	    expr_list[i].type = EXPR_NODE_NAME;
	    i++;
	    break;
	case LEX_LITERAL:
            if (jp_str_cpy(expr_list[i].value, PARSE_BUF_LEN, lex_tok_values[*pos], strlen(lex_tok_values[*pos])) > 0) {
                strncpy(err->msg, "Buffer size exceeded", sizeof(err->msg));                   
                return false;
            }
	    expr_list[i].type = EXPR_LITERAL;
	    i++;
	    break;
	case LEX_LITERAL_BOOL:
		if (jp_str_cpy(expr_list[i].value, PARSE_BUF_LEN, lex_tok_values[*pos], strlen(lex_tok_values[*pos])) > 0) {
			strncpy(err->msg, "Buffer size exceeded", sizeof(err->msg));                   
			return false;
		}
		expr_list[i].type = EXPR_LITERAL_BOOL;
		
		if (strcmp(expr_list[i].value, "true") == 0) {
			jp_str_cpy(expr_list[i].value, PARSE_BUF_LEN, "JP_LITERAL_TRUE", 15);
		} else if (strcmp(expr_list[i].value, "false") == 0) {
			jp_str_cpy(expr_list[i].value, PARSE_BUF_LEN, "JP_LITERAL_FALSE", 16);
		}
		i++;
		break;
	case LEX_LT:
	    expr_list[i].type = EXPR_LT;
            i++;
	    break;
	case LEX_LTE:
	    expr_list[i].type = EXPR_LTE;
	    i++;
	    break;
	case LEX_GT:
	    expr_list[i].type = EXPR_GT;
            i++;
	    break;
	case LEX_GTE:
	    expr_list[i].type = EXPR_GTE;
	    i++;
	    break;
	case LEX_NEQ:
	    expr_list[i].type = EXPR_NE;
	    i++;
	    break;
	case LEX_EQ:
	    expr_list[i].type = EXPR_EQ;
	    i++;
	    break;
	case LEX_OR:
	    if (expr_list[i - 1].type == EXPR_NODE_NAME) {
		expr_list[i].type = EXPR_ISSET;
		i++;
	    }
	    expr_list[i].type = EXPR_OR;
	    i++;
	    break;
	case LEX_AND:
	    if (expr_list[i - 1].type == EXPR_NODE_NAME) {
		expr_list[i].type = EXPR_ISSET;
		i++;
	    }
	    expr_list[i].type = EXPR_AND;
	    i++;
	    break;
        case LEX_RGXP:
            expr_list[i].type = EXPR_RGXP;
            i++;
            break;
        case LEX_NOT_FOUND:
        case LEX_ROOT:
        case LEX_DEEP_SCAN:
        case LEX_SLICE:
        case LEX_CHILD_SEP:
        case LEX_FILTER_START:
        case LEX_ERR:
        default:
            strncpy(err->msg, "Invalid expression token", sizeof(err->msg));
            return false;
	}

	(*pos)++;
    }

    //We made it to the end without finding an expression terminator
    strncpy(err->msg, "Missing filter end ]", sizeof(err->msg));

    return false;
}

bool build_parse_tree(
    lex_token lex_tok[PARSE_BUF_LEN],
    char lex_tok_values[][PARSE_BUF_LEN],
    int lex_tok_count, 
    operator * tok, 
    int *tok_count,
    parse_error * err
) {

    int i = 0, x = 0, z = 0, expr_count = 0;
	int slice_counter = 0;
    int *int_ptr;

    for (i = 0; i < lex_tok_count; i++) {

	switch (lex_tok[i]) {

	case ROOT:
	    tok[x].type = ROOT;
	    x++;
	    break;
	case LEX_WILD_CARD:
	    tok[x].type = WILD_CARD;
	    x++;
	    break;
	case LEX_DEEP_SCAN:
	case LEX_NODE:

	    if (lex_tok[i] == LEX_DEEP_SCAN) {
		tok[x].type = DEEP_SCAN;
		i++;
	    } else {
		tok[x].type = CHILD_KEY;
	    }

	    tok[x].filter_type = FLTR_NODE;
	    tok[x].index_count = 0;
            tok[x].node_value = lex_tok_values[i];
            tok[x].node_value_len = strlen(lex_tok_values[i]);
	    
	    int_ptr = &i;

		if (lex_tok[i + 1] == LEX_EXPR_START) {

                expr_count = get_expression_node_count(&lex_tok[0], *int_ptr, lex_tok_count);

                if (expr_count == -1) {
                    strncpy(err->msg, "Missing filter end ]", sizeof(err->msg));
                    return false;
                }

		i++;
		tok[x].filter_type = FLTR_EXPR;
                tok[x].expressions = (expr_operator *) jpath_malloc(sizeof(expr_operator) * expr_count); 
		if (!tokenize_expression(&lex_tok[0], int_ptr, lex_tok_count, &tok[x], lex_tok_values, err)) {
                   return false;
                }
	    } else if (lex_tok[i + 1] == LEX_FILTER_START) {
		i += 2;
		z = 0;
		slice_counter = 0;
		//TODO What if only 1 element, make sure type doesn't change
		tok[x].filter_type = FLTR_INDEX;
		while (lex_tok[i] != LEX_EXPR_END) {
		    if (lex_tok[i] == LEX_CHILD_SEP) {
			tok[x].filter_type = FLTR_INDEX;
		    } else if (lex_tok[i] == LEX_SLICE) {
			tok[x].filter_type = FLTR_RANGE;
			slice_counter++;
			// [:a] => [0:a]
			// [a::] => [a:0:]
			if (slice_counter > tok[x].index_count) {
				if (slice_counter == 1) {
					tok[x].indexes[z] = 0;
				} else if (slice_counter == 2) {
					tok[x].indexes[z] = 0;
				}
				tok[x].index_count++;
				z++;
			}
		    } else if (lex_tok[i] == LEX_WILD_CARD) {
			tok[x].filter_type = FLTR_WILD_CARD;
		    } else if (lex_tok[i] == LEX_LITERAL) {
			tok[x].indexes[z] = atoi(lex_tok_values[i]);	//TODO error checking
			tok[x].index_count++;
			z++;
		    } else {
                        break;
                    }
		    i++;
		}
		if (lex_tok[i] != LEX_EXPR_END) {
                    strncpy(err->msg, "Missing filter end ]", sizeof(err->msg));
                    return false;
		}
	    }
	    x++;
	    break;
	default:
	    break;
	}
    }

    *tok_count = x;

    return true;
}

operator_type get_token_type(expr_op_type token)
{

    switch (token) {
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_OR:
    case EXPR_AND:
    case EXPR_ISSET:
    case EXPR_RGXP:
	return TYPE_OPERATOR;
    case EXPR_PAREN_LEFT:
    case EXPR_PAREN_RIGHT:
	return TYPE_PAREN;
    case EXPR_LITERAL:
	case EXPR_LITERAL_BOOL:
    case EXPR_NODE_NAME:
    case EXPR_BOOL:
	return TYPE_OPERAND;
    }
}

bool evaluate_postfix_expression(expr_operator * op, int count)
{
    stack s;
    stack_init(&s);
    expr_operator *expr_lh;
    expr_operator *expr_rh;

    /* Temporary operators that store intermediate evaluations */
    expr_operator op_true;

    op_true.type = EXPR_BOOL;
    op_true.value_bool = true;

    expr_operator op_false;

    op_false.type = EXPR_BOOL;
    op_false.value_bool = false;

    int i = 0;

    while (i < count) {

	switch (get_token_type((*op).type)) {
	case TYPE_OPERATOR:

	    if (!is_unary((*op).type)) {
		expr_rh = stack_top(&s);
		stack_pop(&s);
		expr_lh = stack_top(&s);
	    } else {
		expr_rh = stack_top(&s);
		expr_lh = expr_rh;
	    }

	    stack_pop(&s);

	    if (exec_cb_by_token((*op).type) (expr_lh, expr_rh)) {
		stack_push(&s, &op_true);
	    } else {
		stack_push(&s, &op_false);
	    }

	    break;
	case TYPE_OPERAND:
	    stack_push(&s, op);
	    break;
	}

	i++;
	op++;
    }

    expr_lh = stack_top(&s);

    return (*expr_lh).value_bool;
}

// See http://csis.pace.edu/~wolf/CS122/infix-postfix.htm
void convert_to_postfix(expr_operator * expr_in, int in_count, expr_operator * expr_out, int *out_count)
{

    stack s;
    stack_init(&s);
    expr_operator *expr_tmp;

    *out_count = 0;

    int i;

    for (i = 0; i < in_count; i++) {
	switch (get_token_type(expr_in[i].type)) {
	case TYPE_OPERAND:
	    expr_out[(*out_count)++] = expr_in[i];
	    break;
	case TYPE_OPERATOR:
	    if (!s.size || (*stack_top(&s)).type == EXPR_PAREN_LEFT) {
		stack_push(&s, &expr_in[i]);
	    } else {

		expr_tmp = stack_top(&s);

		//TODO compare macro or assign to var?
		if (get_operator_precedence(expr_in[i].type) > get_operator_precedence((*expr_tmp).type)) {
		    stack_push(&s, &expr_in[i]);
		} else if (get_operator_precedence(expr_in[i].type) < get_operator_precedence((*expr_tmp).type)) {
		    expr_out[(*out_count)++] = *stack_top(&s);
		    stack_pop(&s);
		    i--;	//Try the incoming token again in the next loop iteration
		} else {
		    expr_out[(*out_count)++] = *stack_top(&s);
		    stack_pop(&s);
		    stack_push(&s, &expr_in[i]);
		}
	    }
	    break;
	case TYPE_PAREN:
	    if (expr_in[i].type == EXPR_PAREN_LEFT) {
		stack_push(&s, &expr_in[i]);
	    } else {
		while (s.size) {
		    expr_tmp = stack_top(&s);
		    stack_pop(&s);
		    if ((*expr_tmp).type == EXPR_PAREN_LEFT) {
			break;
		    }
		    expr_out[(*out_count)++] = *expr_tmp;
		}
	    }
	    break;
	}
    }

    /* Remove remaining elements */
    for (; s.size; expr_tmp = stack_top(&s), expr_out[(*out_count)++] = *expr_tmp, stack_pop(&s)
	);

}

compare_cb exec_cb_by_token(expr_op_type token_type)
{

    switch (token_type) {
    case EXPR_EQ:
	return compare_eq;
    case EXPR_NE:
        return compare_neq;
    case EXPR_LT:
	return compare_lt;
    case EXPR_LTE:
	return compare_lte;
    case EXPR_GT:
	return compare_gt;
    case EXPR_GTE:
	return compare_gte;
    case EXPR_ISSET:
	return compare_isset;
    case EXPR_OR:
	return compare_or;
    case EXPR_AND:
	return compare_and;
    case EXPR_RGXP:
        return compare_rgxp;
    case EXPR_PAREN_LEFT:
    case EXPR_PAREN_RIGHT:
    case EXPR_LITERAL:
	case EXPR_LITERAL_BOOL:
    case EXPR_BOOL:
    default:
	printf("Error, no callback for token");
	break;
    }
}

bool is_unary(expr_op_type type)
{
    return type == EXPR_ISSET;
}

//TODO: Distinguish between operator and token?
int get_operator_precedence(expr_op_type type)
{

    switch (type) {
    case EXPR_ISSET:
	return 10000;
    case EXPR_LT:
	return 1000;
    case EXPR_LTE:
	return 1000;
	break;
    case EXPR_GT:
	return 1000;
    case EXPR_GTE:
	return 1000;
    case EXPR_RGXP:
        return 1000;
    case EXPR_NE:
	return 900;
    case EXPR_EQ:
	return 900;
    case EXPR_AND:
	return 800;
    case EXPR_OR:
	return 700;
    case EXPR_PAREN_LEFT:
    case EXPR_PAREN_RIGHT:
    case EXPR_LITERAL:
	case EXPR_LITERAL_BOOL:
    case EXPR_BOOL:
    default:
	printf("Error, no callback for token");
	break;
    }
}
