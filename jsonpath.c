
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "src/jsonpath/lexer.h"
#include "src/jsonpath/parser.h"
#include "php_jsonpath.h"
#include "zend_operators.h"
#include <stdbool.h>
#include "zend_exceptions.h"
#include <ext/spl/spl_exceptions.h>
#include <ext/pcre/php_pcre.h>

/* True global resources - no need for thread safety here */
static int le_jsonpath;
void iterate(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC);
void recurse(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC);
bool findByValue(zval * arr, expr_operator * node TSRMLS_DC);
bool checkIfKeyExists(zval * arr, expr_operator * node TSRMLS_DC);
void processChildKey(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC);
void iterateWildCard(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC);
bool is_scalar(zval * arg);

zend_class_entry *test_ce;

PHP_METHOD(JsonPath, find)
{
    char *path;
#if PHP_MAJOR_VERSION < 7
    int path_len;
#else
    size_t path_len;
#endif
    zval *z_array;
    HashTable *arr;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "as", &z_array, &path, &path_len) == FAILURE) {
	return;
    }

    array_init(return_value);

    lex_token lex_tok[PARSE_BUF_LEN];
    char lex_tok_values[PARSE_BUF_LEN][PARSE_BUF_LEN];
    int lex_tok_count = 0;

    char *p = path;

    char buffer[PARSE_BUF_LEN];

    lex_token *ptr = lex_tok;

    lex_error err;

    while ((*ptr = scan(&p, buffer, sizeof(buffer), &err)) != LEX_NOT_FOUND) {
	switch (*ptr) {
	case LEX_NODE:
	case LEX_LITERAL:
	    strcpy(lex_tok_values[lex_tok_count], buffer);
	    break;
        case LEX_ERR:
            snprintf(err.msg, sizeof(err.msg), "%s at position %d", err.msg, (err.pos - path));
            zend_throw_exception(spl_ce_RuntimeException, err.msg, 0 TSRMLS_CC);
            return;
	default:
	    lex_tok_values[lex_tok_count][0] = '\0';
	    break;
	}

	ptr++;

	lex_tok_count++;
    }

    operator  tok[100];
    int tok_count = 0;
    int *int_ptr = &tok_count;
 
    parse_error p_err;
   
    if (!build_parse_tree(lex_tok, lex_tok_values, lex_tok_count, tok, int_ptr, &p_err)) {
        zend_throw_exception(spl_ce_RuntimeException, p_err.msg, 0 TSRMLS_CC);
    }

    operator * tok_ptr_start;
    operator * tok_ptr_end;

    tok_ptr_start = &tok[0];
    tok_ptr_end = &tok[tok_count - 1];

    iterate(z_array, tok_ptr_start, tok_ptr_end, return_value TSRMLS_CC);

    operator * fr = tok_ptr_start;

    while (fr <= tok_ptr_end) {
        if (fr->filter_type == FLTR_EXPR) {
            efree((void *)fr->expressions);    
        } 
        fr++;
    }

    if (zend_hash_num_elements(HASH_OF(return_value)) == 0) {
	convert_to_boolean(return_value);
	RETURN_FALSE;
    }

    return;
}

void iterate(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC)
{
    if (tok > tok_last) {
	return;
    }

    switch (tok->type) {
    case ROOT:
	iterate(arr, (tok + 1), tok_last, return_value TSRMLS_CC);
	break;
    case WILD_CARD:
	iterateWildCard(arr, tok, tok_last, return_value TSRMLS_CC);
	return;
    case DEEP_SCAN:
	recurse(arr, tok, tok_last, return_value TSRMLS_CC);
	return;
    case CHILD_KEY:
	processChildKey(arr, tok, tok_last, return_value TSRMLS_CC);
	return;
    }
}

#if PHP_MAJOR_VERSION < 7
void copyToReturnResult(zval ** arr, zval * return_value)
{
    zval *zv_dest;
    ALLOC_ZVAL(zv_dest);
    MAKE_COPY_ZVAL(arr, zv_dest);
    add_next_index_zval(return_value, zv_dest);
}
#else
void copyToReturnResult(zval * arr, zval * return_value)
{
    zval tmp;
    ZVAL_COPY_VALUE(&tmp, arr);
    zval_copy_ctor(&tmp);
    add_next_index_zval(return_value, &tmp);
}
#endif

void processChildKey(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC)
{
#if PHP_MAJOR_VERSION < 7
    zval **data, **data2;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
	return;
    }

    if (zend_hash_find(HASH_OF(arr), tok->node_value, strlen(tok->node_value) + 1, (void **) &data) != SUCCESS) {
	return;
    }

    int x;
    HashPosition pos;

    switch (tok->filter_type) {
    case FLTR_RANGE:
	for (x = tok->indexes[0]; x < tok->indexes[1]; x++) {
	    if (zend_hash_index_find(HASH_OF(*data), x, (void **) &data2) == SUCCESS) {
		if (tok == tok_last) {
		    copyToReturnResult(data2, return_value);
		} else {
		    iterate(*data2, (tok + 1), tok_last, return_value TSRMLS_CC);
		}
	    }
	}
	return;
    case FLTR_INDEX:
	for (x = 0; x < tok->index_count; x++) {
	    if (zend_hash_index_find(HASH_OF(*data), tok->indexes[x], (void **) &data2) == SUCCESS) {
		if (tok == tok_last) {
		    copyToReturnResult(data2, return_value);
		} else {
		    iterate(*data2, (tok + 1), tok_last, return_value TSRMLS_CC);
		}
	    }
	}
	return;
    case FLTR_WILD_CARD:
	for (zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
	     zend_hash_get_current_data_ex(HASH_OF(*data), (void **) &data2, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(HASH_OF(*data), &pos)
	    ) {
	    if (tok == tok_last) {
		copyToReturnResult(data2, return_value);
	    } else {
		iterate(*data2, (tok + 1), tok_last, return_value TSRMLS_CC);
	    }
	}
	return;
    case FLTR_NODE:
	if (tok == tok_last) {
	    copyToReturnResult(data, return_value);
	} else {
	    iterate(*data, (tok + 1), tok_last, return_value TSRMLS_CC);
	}
	return;
    case FLTR_EXPR:
	for (zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
	     zend_hash_get_current_data_ex(HASH_OF(*data), (void **) &data2, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(HASH_OF(*data), &pos)
	    ) {
	    // For each array entry, find the node names and populate their values
	    // Fill up expression NODE_NAME VALS
	    for (x = 0; x < tok->expression_count; x++) {
		if (tok->expressions[x + 1].type == EXPR_ISSET) {
		    if (!checkIfKeyExists(*data2, &tok->expressions[x] TSRMLS_CC)) {
			continue;
		    }
		} else if (tok->expressions[x].type == EXPR_NODE_NAME) {
		    if (!findByValue(*data2, &tok->expressions[x] TSRMLS_CC)) {
			continue;
		    }
		}
	    }

	    if (evaluate_postfix_expression(tok->expressions, tok->expression_count)) {
		if (tok == tok_last) {
		    copyToReturnResult(data2, return_value);
		} else {
		    iterate(*data2, (tok + 1), tok_last, return_value TSRMLS_CC);
		}
	    }
	}
	break;
    }
#else
    zval *data, *data2;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
	return;
    }

    if ((data = zend_hash_str_find(HASH_OF(arr), tok->node_value, tok->node_value_len)) == NULL) {
	return;
    }

    int x;
    zend_string *key;
    ulong num_key;

    switch (tok->filter_type) {
    case FLTR_RANGE:
	for (x = tok->indexes[0]; x < tok->indexes[1]; x++) {
	    if ((data2 = zend_hash_index_find(HASH_OF(data), x)) != NULL) {
		if (tok == tok_last) {
		    copyToReturnResult(data2, return_value);
		} else {
		    iterate(data2, (tok + 1), tok_last, return_value TSRMLS_CC);
		}
	    }
	}
	return;
    case FLTR_INDEX:
	for (x = 0; x < tok->index_count; x++) {
	    if ((data2 = zend_hash_index_find(HASH_OF(data), tok->indexes[x])) != NULL) {
		if (tok == tok_last) {
		    copyToReturnResult(data2, return_value);
		} else {
		    iterate(data2, (tok + 1), tok_last, return_value TSRMLS_CC);
		}
	    }
	}
	return;
    case FLTR_WILD_CARD:

	ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(data), num_key, key, data2) {
	    if (tok == tok_last) {
		copyToReturnResult(data2, return_value);
	    } else {
		iterate(data2, (tok + 1), tok_last, return_value TSRMLS_CC);
	    }
	}
	ZEND_HASH_FOREACH_END();

	return;
    case FLTR_NODE:
	if (tok == tok_last) {
	    copyToReturnResult(data, return_value);
	} else {
	    iterate(data, (tok + 1), tok_last, return_value TSRMLS_CC);
	}
	return;
    case FLTR_EXPR:

	ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(data), num_key, key, data2) {
	    // For each array entry, find the node names and populate their values
	    // Fill up expression NODE_NAME VALS
	    for (x = 0; x < tok->expression_count; x++) {
		if (tok->expressions[x + 1].type == EXPR_ISSET) {
		    if (!checkIfKeyExists(data2, &tok->expressions[x] TSRMLS_CC)) {
			continue;
		    }
		} else if (tok->expressions[x].type == EXPR_NODE_NAME) {
		    if (!findByValue(data2, &tok->expressions[x] TSRMLS_CC)) {
			continue;
		    }
		}
	    }

	    if (evaluate_postfix_expression(tok->expressions, tok->expression_count)) {
		if (tok == tok_last) {
		    copyToReturnResult(data2, return_value);
		} else {
		    iterate(data2, (tok + 1), tok_last, return_value TSRMLS_CC);
		}
	    }
	}
	ZEND_HASH_FOREACH_END();

	break;
    }
#endif
}

void iterateWildCard(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC)
{
#if PHP_MAJOR_VERSION < 7
    zval **data;
    HashPosition pos;
    zval *zv_dest;

    for (zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
	 zend_hash_get_current_data_ex(HASH_OF(arr), (void **) &data, &pos) == SUCCESS;
	 zend_hash_move_forward_ex(HASH_OF(arr), &pos)
	) {
	if (tok == tok_last) {
	    copyToReturnResult(data, return_value);
	} else if (Z_TYPE_P(*data) == IS_ARRAY) {
	    iterate(*data, (tok + 1), tok_last, return_value TSRMLS_CC);
	}
    }
#else
    zval *data;
    zval *zv_dest;
    zend_string *key;
    ulong num_key;

    ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
	if (tok == tok_last) {
	    copyToReturnResult(data, return_value);
	} else if (Z_TYPE_P(data) == IS_ARRAY) {
	    iterate(data, (tok + 1), tok_last, return_value TSRMLS_CC);
	}
    }
    ZEND_HASH_FOREACH_END();
#endif
}

void recurse(zval * arr, operator * tok, operator * tok_last, zval * return_value TSRMLS_DC)
{
    if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
	return;
    }

    processChildKey(arr, tok, tok_last, return_value TSRMLS_CC);

#if PHP_MAJOR_VERSION < 7
    zval **tmp;
    HashPosition pos;

    for (zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
	 zend_hash_get_current_data_ex(HASH_OF(arr), (void **) &tmp, &pos) == SUCCESS;
	 zend_hash_move_forward_ex(HASH_OF(arr), &pos)
	) {
	recurse(*tmp, tok, tok_last, return_value TSRMLS_CC);
    }
#else
    zval *data;
    zval *zv_dest;
    zend_string *key;
    ulong num_key;

    ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
	recurse(data, tok, tok_last, return_value TSRMLS_CC);
    }
    ZEND_HASH_FOREACH_END();
#endif
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
bool findByValue(zval * arr, expr_operator * node TSRMLS_DC)
{
    if (Z_TYPE_P(arr) != IS_ARRAY) {
	return;
    }

#if PHP_MAJOR_VERSION < 7
    zval **data;

    int i;

    for (i = 0; i < node->label_count; i++) {
	if (zend_hash_find(HASH_OF(arr), node->label[i], strlen(node->label[i]) + 1, (void **) &data) == SUCCESS) {
	    arr = *data;
	} else {
	    node->value[0] = '\0';
	    return false;
	}
    }

    if (!is_scalar(*data)) {
        return false;
    }

    char *s = NULL;
    size_t s_len;

    if (Z_TYPE_P(*data) != IS_STRING) {

        zval *zvp, zcopy;
        zvp = *data;
        int free_zcopy;

        zend_make_printable_zval(zvp, &zcopy, &free_zcopy);
        
        if (free_zcopy) {
            zvp = &zcopy;
        }

        s_len = Z_STRLEN_P(zvp);
        s = Z_STRVAL_P(zvp);

        strncpy(node->value, s, s_len);
        node->value[s_len] = '\0';

        if (free_zcopy) {
           zval_dtor(&zcopy);
        }

    } else {
        s_len = Z_STRLEN_P(*data);
        s = Z_STRVAL_P(*data);
        strncpy(node->value, s, s_len);
        node->value[s_len] = '\0';
    }
#else
    zval *data;

    int i;

    for (i = 0; i < node->label_count; i++) {

	if ((data = zend_hash_str_find(HASH_OF(arr), node->label[i], strlen(node->label[i]))) != NULL) {
	    arr = data;
	} else {
	    node->value[0] = '\0';
	    return false;
	}
    }

    if (!is_scalar(data)) {
        return false;
    }

    char *s = NULL;
    size_t s_len;

    if (Z_TYPE_P(data) != IS_STRING) {

        zval zcopy;
        int free_zcopy;
        char *s = NULL;
	size_t s_len;

        free_zcopy = zend_make_printable_zval(data, &zcopy);
        if (free_zcopy) {
            data = &zcopy;
        }

        s_len = Z_STRLEN_P(data);
        s = Z_STRVAL_P(data);

        strncpy(node->value, s, s_len);
        node->value[s_len] = '\0';

        if (free_zcopy) {
            zval_dtor(&zcopy);
        }

    } else {
        s_len = Z_STRLEN_P(data);
        s = Z_STRVAL_P(data);
        strncpy(node->value, s, s_len);
        node->value[s_len] = '\0';
    }
#endif
    return true;
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
bool checkIfKeyExists(zval * arr, expr_operator * node TSRMLS_DC)
{
#if PHP_MAJOR_VERSION < 7
    zval **data;

    HashPosition pos;		/* hash iterator */

    int i;

    node->value_bool = false;

    for (i = 0; i < node->label_count; i++) {
	if (zend_hash_find(HASH_OF(arr), node->label[i], strlen(node->label[i]) + 1, (void **) &data) != SUCCESS) {
	    node->value_bool = false;
	    return false;
	} else {
	    node->value_bool = true;
	    arr = *data;
	}
    }
#else
    zval *data;

    int i;

    node->value_bool = false;

    for (i = 0; i < node->label_count; i++) {

	if ((data = zend_hash_str_find(HASH_OF(arr), node->label[i], strlen(node->label[i]))) == NULL) {
	    node->value_bool = false;
	    return false;
	} else {
	    node->value_bool = true;
	    arr = data;
	}
    }
#endif
    return true;
}

bool compare_lt(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    bool res = (Z_LVAL_P(result) < 0);

    FREE_ZVAL(a);
    FREE_ZVAL(b);
    FREE_ZVAL(result);
#else

    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);
    compare_function(&result, &a, &b TSRMLS_CC);

    bool res = (Z_LVAL(result) < 0);

#endif
    return res;
}

bool compare_gt(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    bool res = (Z_LVAL_P(result) > 0);

    FREE_ZVAL(a);
    FREE_ZVAL(b);
    FREE_ZVAL(result);
#else

    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);
    compare_function(&result, &a, &b TSRMLS_CC);

    bool res = (Z_LVAL(result) > 0);

#endif
    return res;
}

bool compare_lte(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    bool res = (Z_LVAL_P(result) <= 0);

    FREE_ZVAL(a);
    FREE_ZVAL(b);
    FREE_ZVAL(result);
#else

    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);
    compare_function(&result, &a, &b TSRMLS_CC);

    bool res = (Z_LVAL(result) <= 0);

#endif
    return res;
}

bool compare_gte(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    bool res = (Z_LVAL_P(result) >= 0);

    FREE_ZVAL(a);
    FREE_ZVAL(b);
    FREE_ZVAL(result);
#else

    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);
    compare_function(&result, &a, &b TSRMLS_CC);

    bool res = (Z_LVAL(result) >= 0);

#endif
    return res;
}

bool compare_and(expr_operator * lh, expr_operator * rh)
{
    return (*lh).value_bool && (*rh).value_bool;
}

bool compare_or(expr_operator * lh, expr_operator * rh)
{
    return (*lh).value_bool || (*rh).value_bool;
}

bool compare_eq(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    bool res = (Z_LVAL_P(result) == 0);

    FREE_ZVAL(a);
    FREE_ZVAL(b);
    FREE_ZVAL(result);
#else

    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);
    compare_function(&result, &a, &b TSRMLS_CC);

    bool res = (Z_LVAL(result) == 0);

#endif
    return res;
}

bool compare_neq(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    bool res = (Z_LVAL_P(result) != 0);

    FREE_ZVAL(a);
    FREE_ZVAL(b);
    FREE_ZVAL(result);
#else

    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);
    compare_function(&result, &a, &b TSRMLS_CC);

    bool res = (Z_LVAL(result) != 0);

#endif
    return res;
}

bool compare_isset(expr_operator * lh, expr_operator * rh)
{
    return (*lh).value_bool && (*rh).value_bool;
}

bool compare_rgxp(expr_operator * lh, expr_operator * rh)
{
    TSRMLS_FETCH();

#if PHP_MAJOR_VERSION < 7

    zval * pattern;
    pcre_cache_entry *pce;

    MAKE_STD_ZVAL(pattern);
    ZVAL_STRINGL(pattern, (*rh).value, strlen((*rh).value), 0);

    if ((pce = pcre_get_compiled_regex_cache(Z_STRVAL_P(pattern), Z_STRLEN_P(pattern) TSRMLS_CC)) == NULL) {
        zval_dtor(pattern);
        FREE_ZVAL(pattern);
        return false;
    }

    zval * retval;
    zval * subpats;

    MAKE_STD_ZVAL(retval);
    ALLOC_INIT_ZVAL(subpats);

    php_pcre_match_impl(pce, (*lh).value, strlen((*lh).value), retval, subpats, 0, 0, 0, 0 TSRMLS_CC);

    long ret = Z_LVAL_P(retval);

    zval_ptr_dtor(&subpats);
    FREE_ZVAL(retval);
    FREE_ZVAL(pattern);

    return ret > 0;
#else
    zval pattern;
    pcre_cache_entry *pce;

    ZVAL_STRING(&pattern, (*rh).value);

    if ((pce = pcre_get_compiled_regex_cache(Z_STR(pattern))) == NULL) {
        zval_ptr_dtor(&pattern);
        return false;
    }

    zval retval;
    zval subpats;

    ZVAL_NULL(&retval);
    ZVAL_NULL(&subpats);

    php_pcre_match_impl(pce, (*lh).value, strlen((*lh).value), &retval, &subpats, 0, 0, 0, 0);

    zval_ptr_dtor(&subpats);
    zval_ptr_dtor(&pattern);
    return Z_LVAL(retval) > 0;
#endif
}

bool is_scalar(zval * arg)
{
    switch (Z_TYPE_P(arg)) {
#if PHP_MAJOR_VERSION < 7
        case IS_BOOL:
#else
        case IS_FALSE:
        case IS_TRUE:
#endif
        case IS_DOUBLE:
        case IS_LONG:
        case IS_STRING:
        return true;
        break;
        
        default:
        return false;
        break;
    }
}

void * jpath_malloc(size_t size) {
    return emalloc(size);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_find, 0, 0, 2)
    ZEND_ARG_ARRAY_INFO(0, haystack, 0)
    ZEND_ARG_INFO(0, needle)
    ZEND_END_ARG_INFO()

const zend_function_entry jsonpath_methods[] = {
    PHP_ME(JsonPath, find, arginfo_find, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jsonpath)
{
    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "JsonPath", jsonpath_methods);

    test_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);

    return SUCCESS;
}

/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(jsonpath)
{
    /* uncomment this line if you have INI entries
       UNREGISTER_INI_ENTRIES();
     */
    return SUCCESS;
}

/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(jsonpath)
{
    return SUCCESS;
}

/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(jsonpath)
{
    return SUCCESS;
}

/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(jsonpath)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "jsonpath support", "enabled");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
       DISPLAY_INI_ENTRIES();
     */
}

/* }}} */

/* {{{ jsonpath_functions[]
 *
 * Every user visible function must have an entry in jsonpath_functions[].
 */
const zend_function_entry jsonpath_functions[] = {
    PHP_FE_END			/* Must be the last line in jsonpath_functions[] */
};

/* }}} */

/* {{{ jsonpath_module_entry
 */
zend_module_entry jsonpath_module_entry = {
    STANDARD_MODULE_HEADER,
    "jsonpath",
    jsonpath_functions,
    PHP_MINIT(jsonpath),
    PHP_MSHUTDOWN(jsonpath),
    PHP_RINIT(jsonpath),	/* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(jsonpath),	/* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(jsonpath),
    PHP_JSONPATH_VERSION,
    STANDARD_MODULE_PROPERTIES
};

/* }}} */

#ifdef COMPILE_DL_JSONPATH
ZEND_GET_MODULE(jsonpath)
#endif
