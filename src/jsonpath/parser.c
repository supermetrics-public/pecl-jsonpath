#include "lexer.h"
#include "parser.h"
#include "stack.h"
#include <stdio.h>

bool is_unary(expr_op_type);

void tokenize_filter_expression(lex_token * lex_tok, int *pos, operator * tok, char lex_tok_values[100][100]
    )
{

    expr_operator expr_list[100];
    int i = 0, x = 0;

    //TODO make this safer in case LEX_EXPR_END doesnt exist
    while (lex_tok[*pos] != LEX_EXPR_END) {

	switch (lex_tok[*pos]) {

	case LEX_CUR_NODE:
	    x = 0;
	    expr_list[i].label_count = 0;
	    while (lex_tok[(*pos) + 1] == LEX_NODE) {
		(*pos)++;
		strcpy(expr_list[i].label[x], lex_tok_values[*pos]);
		expr_list[i].label_count++;
		x++;
	    }
	    expr_list[i].type = EXPR_NODE_NAME;
	    i++;
	    break;
	case LEX_LITERAL:
	    strcpy(expr_list[i].value, lex_tok_values[*pos]);
	    expr_list[i].type = EXPR_LITERAL;
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
	default:
	    break;
	}

	(*pos)++;
    }

    convert_to_postfix(expr_list, i, tok->expressions, &tok->expression_count);
}

void build_parse_tree(lex_token lex_tok[100],
		      char lex_tok_values[100][100], int lex_tok_count, operator * tok, int *tok_count)
{

    int i = 0, x = 0, z = 0;
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
	    strcpy(tok[x].node_value, lex_tok_values[i]);

	    int_ptr = &i;

	    if (lex_tok[i + 1] == LEX_EXPR_START) {

		i++;
		tok[x].filter_type = FLTR_EXPR;

		tokenize_filter_expression(&lex_tok[0], int_ptr, &tok[x], lex_tok_values);
	    } else if (lex_tok[i + 1] == LEX_FILTER_START) {
		i++;
		z = 0;
		//TODO What if only 1 element, make sure type doesn't change
		tok[x].filter_type = FLTR_INDEX;
		while (lex_tok[i] != LEX_EXPR_END) {
		    if (lex_tok[i] == LEX_CHILD_SEP) {
			tok[x].filter_type = FLTR_INDEX;
		    } else if (lex_tok[i] == LEX_SLICE) {
			tok[x].filter_type = FLTR_RANGE;
		    } else if (lex_tok[i] == LEX_WILD_CARD) {
			tok[x].filter_type = FLTR_WILD_CARD;
		    } else if (lex_tok[i] == LEX_LITERAL) {
			tok[x].indexes[z] = atoi(lex_tok_values[i]);	//TODO error checking
			tok[x].index_count++;
			z++;
		    }
		    i++;
		}
		if (lex_tok[i] != LEX_EXPR_END) {
		    /* Throw parsing error */
		}
	    }
	    x++;
	    break;
	default:
	    break;
	}
    }

    *tok_count = x;
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
	return TYPE_OPERATOR;
    case EXPR_PAREN_LEFT:
    case EXPR_PAREN_RIGHT:
	return TYPE_PAREN;
    case EXPR_LITERAL:
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
		if (get_operator_precedence(expr_in[i].type) < get_operator_precedence((*expr_tmp).type)) {
		    stack_push(&s, &expr_in[i]);
		} else if (get_operator_precedence(expr_in[i].type) > get_operator_precedence((*expr_tmp).type)) {
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
	printf("Callback not supported yet");
	break;
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
    case EXPR_PAREN_LEFT:
    case EXPR_PAREN_RIGHT:
    case EXPR_LITERAL:
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
    case EXPR_LT:
	return 0;
    case EXPR_LTE:
	return 0;
	break;
    case EXPR_GT:
	return 0;
    case EXPR_GTE:
	return 0;
    case EXPR_NE:
	return 1;
    case EXPR_EQ:
	return 1;
    case EXPR_AND:
	return 2;
    case EXPR_OR:
	return 2;
    case EXPR_ISSET:
	return -1;
    case EXPR_PAREN_LEFT:
    case EXPR_PAREN_RIGHT:
    case EXPR_LITERAL:
    case EXPR_BOOL:
    default:
	printf("Error, no callback for token");
	break;
    }
}
