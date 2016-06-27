#include "lexer.h"
#include "parser.h"
#include <stdio.h>

bool is_unary(token token_type);

void tokenize_filter_expression(lex_token * lex_tok, int *pos, struct token *tok, char lex_tok_values[100][100]
    )
{

    expr expr_list[100];
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
	    expr_list[i].type = NODE_NAME;
	    i++;
	    break;
	case LEX_LITERAL:
	    strcpy(expr_list[i].value, lex_tok_values[*pos]);
	    expr_list[i].type = LITERAL;
	    i++;
	    break;
	case LEX_LT:
	    expr_list[i].type = LT;
	    break;
	case LEX_LTE:
	    expr_list[i].type = LTE;
	    i++;
	    break;
	case LEX_GT:
	    expr_list[i].type = GT;
	    break;
	case LEX_GTE:
	    expr_list[i].type = GTE;
	    i++;
	    break;
	case LEX_NEQ:
	    expr_list[i].type = NE;
	    i++;
	    break;
	case LEX_EQ:
	    expr_list[i].type = EQ;
	    i++;
	    break;
	case LEX_OR:
	    if (expr_list[i - 1].type == NODE_NAME) {
		expr_list[i].type = ISSET;
		i++;
	    }
	    expr_list[i].type = OR;
	    i++;
	    break;
	case LEX_AND:
	    if (expr_list[i - 1].type == NODE_NAME) {
		expr_list[i].type = ISSET;
		i++;
	    }
	    expr_list[i].type = AND;
	    i++;
	    break;
	default:
	    break;
	}

	(*pos)++;
    }

    convert_to_postfix(expr_list, i, tok->prop.expr_list, &tok->prop.expr_count);
}

void build_parse_tree(lex_token lex_tok[100],
		      char lex_tok_values[100][100], int lex_tok_count, struct token *tok, int *tok_count)
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

	    tok[x].prop.type = FLTR_NODE;
	    tok[x].prop.index_count = 0;
	    strcpy(tok[x].prop.val, lex_tok_values[i]);


	    int_ptr = &i;

	    if (lex_tok[i + 1] == LEX_EXPR_START) {

		i++;
		tok[x].prop.type = FLTR_EXPR;

		tokenize_filter_expression(&lex_tok[0], int_ptr, &tok[x], lex_tok_values);
	    } else if (lex_tok[i + 1] == LEX_FILTER_START) {
		i++;
		z = 0;
		//TODO What if only 1 element, make sure type doesn't change
		tok[x].prop.type = FLTR_INDEX;
		while (lex_tok[i] != LEX_EXPR_END) {
		    if (lex_tok[i] == LEX_CHILD_SEP) {
			tok[x].prop.type = FLTR_INDEX;
		    } else if (lex_tok[i] == LEX_SLICE) {
			tok[x].prop.type = FLTR_RANGE;
		    } else if (lex_tok[i] == LEX_WILD_CARD) {
			tok[x].prop.type = FLTR_WILD_CARD;
		    } else if (lex_tok[i] == LEX_LITERAL) {
			tok[x].prop.indexes[z] = atoi(lex_tok_values[i]);	//TODO error checking
			tok[x].prop.index_count++;
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

void Stack_Init(Stack * S)
{
    S->size = 0;
}

expr *Stack_Top(Stack * S)
{
    if (S->size == 0) {
	fprintf(stderr, "Error: stack empty\n");
	return NULL;
    }

    return S->data[S->size - 1];
}

void Stack_Push(Stack * S, expr * expr)
{
    if (S->size < STACK_MAX)
	S->data[S->size++] = expr;
    else
	fprintf(stderr, "Error: stack full\n");
}

void Stack_Pop(Stack * S)
{
    if (S->size == 0)
	fprintf(stderr, "Error: stack empty\n");
    else
	S->size--;
}

token_type get_token_type(token token)
{

    switch (token) {
    case EQ:
    case NE:
    case LT:
    case LTE:
    case GT:
    case GTE:
    case OR:
    case AND:
    case ISSET:
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

bool evaluate_postfix_expression(expr * expression_original, int count)
{

    expr expression[100] = { 0 };

    memcpy(expression, expression_original, sizeof(expr) * count);

    Stack S;
    Stack_Init(&S);
    expr *expr_lh;
    expr *expr_rh;
    bool temp_res;
    int i;

    for (i = 0; i < count; i++) {

	switch (get_token_type(expression[i].type)) {
	case TYPE_OPERATOR:

	    if (!is_unary(expression[i].type)) {
		expr_rh = Stack_Top(&S);
		Stack_Pop(&S);
		expr_lh = Stack_Top(&S);
	    } else {
		expr_rh = Stack_Top(&S);
		expr_lh = expr_rh;
	    }

	    temp_res = exec_cb_by_token(expression[i].type) (expr_lh, expr_rh);

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
void convert_to_postfix(expr * expr_in, int in_count, expr * expr_out, int *out_count)
{

    Stack S;
    Stack_Init(&S);
    expr *expr_tmp;

    *out_count = 0;

    int i;

    for (i = 0; i < in_count; i++) {
	switch (get_token_type(expr_in[i].type)) {
	case TYPE_OPERAND:
	    expr_out[(*out_count)++] = expr_in[i];
	    break;
	case TYPE_OPERATOR:
	    if (!S.size || (*Stack_Top(&S)).type == PAREN_LEFT) {
		Stack_Push(&S, &expr_in[i]);
	    } else {

		expr_tmp = Stack_Top(&S);

		//TODO compare macro or assign to var?
		if (get_operator_precedence(expr_in[i].type) < get_operator_precedence((*expr_tmp).type)) {
		    Stack_Push(&S, &expr_in[i]);
		} else if (get_operator_precedence(expr_in[i].type) > get_operator_precedence((*expr_tmp).type)) {
		    expr_out[(*out_count)++] = *Stack_Top(&S);
		    Stack_Pop(&S);
		    i--;	//Try the incoming token again in the next loop iteration
		} else {
		    expr_out[(*out_count)++] = *Stack_Top(&S);
		    Stack_Pop(&S);
		    Stack_Push(&S, &expr_in[i]);
		}
	    }
	    break;
	case TYPE_PAREN:
	    if (expr_in[i].type == PAREN_LEFT) {
		Stack_Push(&S, &expr_in[i]);
	    } else {
		while (S.size) {
		    expr_tmp = Stack_Top(&S);
		    Stack_Pop(&S);
		    if ((*expr_tmp).type == PAREN_LEFT) {
			break;
		    }
		    expr_out[(*out_count)++] = *expr_tmp;
		}
	    }
	    break;
	}
    }

    for (; S.size; expr_tmp = Stack_Top(&S), expr_out[(*out_count)++] = *expr_tmp, Stack_Pop(&S)
	);

}

compare_cb exec_cb_by_token(token token_type)
{

    switch (token_type) {
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
	return isset2;
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

bool is_unary(token token_type)
{
    return token_type == ISSET;
}

//TODO: Distinguish between operator and token?
int get_operator_precedence(token token_type)
{

    switch (token_type) {
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
	return -1;
    case PAREN_LEFT:
    case PAREN_RIGHT:
    case LITERAL:
    case BOOL:
    default:
	printf("Error, no callback for token");
	break;
    }
}
