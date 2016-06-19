/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

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

/* True global resources - no need for thread safety here */
static int le_jsonpath;

void iterate(zval *arr, struct token * tok, struct token * tok_last, zval * return_value);
void deepJump(zval * arr, struct token * tok, struct token * tok_last, zval * return_value);
bool findByValue(zval *arr, expr * node);
bool checkIfKeyExists(zval *arr, expr * node);
void processChildKey(zval *arr, struct token * tok, struct token * tok_last, zval * return_value);
void iterateWildCard(zval * arr, struct token * tok, struct token * tok_last, zval * return_value);

PHP_FUNCTION(path_lookup)
{
    char * path;
#if PHP_MAJOR_VERSION < 7
    int path_len;
#else
    size_t path_len;
#endif
    zval *z_array;
    HashTable *arr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as", &z_array, &path, &path_len) == FAILURE) {
        return;
    }

    array_init(return_value);

    lex_token lex_tok[100];
    char lex_tok_values[100][100];
    int lex_tok_count = 0;

    char * p = path;

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

    struct token * tok_ptr_start;
    struct token * tok_ptr_end;

    tok_ptr_start = &tok[0];
    tok_ptr_end = &tok[tok_count-1];


    iterate(z_array, tok_ptr_start, tok_ptr_end, return_value);

    if(zend_hash_num_elements(HASH_OF(return_value)) == 0) {
#if PHP_MAJOR_VERSION < 7
	FREE_ZVAL(return_value);
#else
	zval_ptr_dtor(&return_value);
#endif
	RETURN_FALSE;
    }

    return;
}

void iterate(zval *arr, struct token * tok, struct token * tok_last, zval * return_value)
{
    if(tok > tok_last) {
        return;
    }

    switch(tok->type) {
        case ROOT:
            iterate(arr, (tok + 1), tok_last, return_value);
            break;
        case WILD_CARD:
            iterateWildCard(arr, tok, tok_last, return_value);
            return;
        case DEEP_SCAN:
            deepJump(arr, tok, tok_last, return_value);
            return;
        case CHILD_KEY:
            processChildKey(arr, tok, tok_last, return_value);
            return;
    }
}

#if PHP_MAJOR_VERSION < 7
void copyToReturnResult(zval **arr, zval * return_value)
{
    zval *zv_dest;
    ALLOC_ZVAL(zv_dest);
    MAKE_COPY_ZVAL(arr, zv_dest);
    add_next_index_zval(return_value, zv_dest);
}
#else
void copyToReturnResult(zval *arr, zval * return_value)
{
    zval tmp;
    ZVAL_COPY_VALUE(&tmp, arr);
    zval_copy_ctor(&tmp);
    add_next_index_zval(return_value, &tmp);
}
#endif

void processChildKey(zval *arr, struct token * tok, struct token * tok_last, zval * return_value)
{
#if PHP_MAJOR_VERSION < 7
    zval **data, **data2;

    if(zend_hash_find(HASH_OF(arr), tok->prop.val, strlen(tok->prop.val) + 1, (void**)&data) != SUCCESS) {
        return;
    }

    int x;
    HashPosition pos;

    switch(tok->prop.type) {
        case RANGE:
            for(x = tok->prop.indexes[0]; x < tok->prop.indexes[1]; x++) {
                if(zend_hash_index_find(HASH_OF(*data), x, (void**)&data2) == SUCCESS) {
                    if(tok == tok_last) {
                        copyToReturnResult(data2, return_value);
                    } else {
                        iterate(*data2, (tok + 1), tok_last, return_value);
                    }
                }
            }
            return;
        case INDEX:
            for(x = 0; x < tok->prop.index_count; x++) {
                if(zend_hash_index_find(HASH_OF(*data), tok->prop.indexes[x], (void**)&data2) == SUCCESS) {
                    if(tok == tok_last) {
                        copyToReturnResult(data2, return_value);
                    } else {
                        iterate(*data2, (tok + 1), tok_last, return_value);
                    }
                }
            }
            return;
        case ANY:
            for(
                zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
                zend_hash_get_current_data_ex(HASH_OF(*data), (void**) &data2, &pos) == SUCCESS;
                zend_hash_move_forward_ex(HASH_OF(*data), &pos)
            ) {
                if(tok == tok_last) {
                    copyToReturnResult(data2, return_value);
                } else {
                    iterate(*data2, (tok + 1), tok_last, return_value);
                }
            }
            return;
        case SINGLE_KEY:
            if(tok == tok_last) {
                copyToReturnResult(data, return_value);
            } else {
                iterate(*data, (tok + 1), tok_last, return_value);
            }
            return;
        case FILTER:
            for(
                zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
                zend_hash_get_current_data_ex(HASH_OF(*data), (void**) &data2, &pos) == SUCCESS;
                zend_hash_move_forward_ex(HASH_OF(*data), &pos)
            ) {
                // For each array entry, find the node names and populate their values
                // Fill up expression NODE_NAME VALS
                for(x = 0; x < tok->prop.expr_count; x++) {
                    if(tok->prop.expr_list[x+1].type == ISSET) {
                        if(!checkIfKeyExists(*data2, &tok->prop.expr_list[x])) {
                            continue;
                        }
                    } else if(tok->prop.expr_list[x].type == NODE_NAME) {
                        if(!findByValue(*data2, &tok->prop.expr_list[x])) {
                            continue;
                        }
                    }
                }

                if(evaluate_postfix_expression(tok->prop.expr_list, tok->prop.expr_count)) {
                    copyToReturnResult(data2, return_value);
                }
            }
            break;
    }
#else
    zval *data, *data2;

    if ((data = zend_hash_str_find(HASH_OF(arr), tok->prop.val, strlen(tok->prop.val))) == NULL) {
        return;
    }

    int x;
    zend_string *key;
    ulong num_key;

    switch(tok->prop.type) {
        case RANGE:
            for(x = tok->prop.indexes[0]; x < tok->prop.indexes[1]; x++) {
                if ((data2 = zend_hash_index_find(HASH_OF(data), x)) != NULL) {
                    if(tok == tok_last) {
                        copyToReturnResult(data2, return_value);
                    } else {
                        iterate(data2, (tok + 1), tok_last, return_value);
                    }
                }
            }
            return;
        case INDEX:
            for(x = 0; x < tok->prop.index_count; x++) {
                if ((data2 = zend_hash_index_find(HASH_OF(data), tok->prop.indexes[x])) != NULL) {
                    if(tok == tok_last) {
                        copyToReturnResult(data2, return_value);
                    } else {
                        iterate(data2, (tok + 1), tok_last, return_value);
                    }
                }
            }
            return;
        case ANY:

            ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(data), num_key, key, data2) {
                if(tok == tok_last) {
                    copyToReturnResult(data2, return_value);
                } else {
                    iterate(data2, (tok + 1), tok_last, return_value);
                }
            } ZEND_HASH_FOREACH_END();

            return;
        case SINGLE_KEY:
            if(tok == tok_last) {
                copyToReturnResult(data, return_value);
            } else {
                iterate(data, (tok + 1), tok_last, return_value);
            }
            return;
        case FILTER:

            ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(data), num_key, key, data2) {
                // For each array entry, find the node names and populate their values
                // Fill up expression NODE_NAME VALS
                for(x = 0; x < tok->prop.expr_count; x++) {
                    if(tok->prop.expr_list[x+1].type == ISSET) {
                        if(!checkIfKeyExists(data2, &tok->prop.expr_list[x])) {
                            continue;
                        }
                    } else if(tok->prop.expr_list[x].type == NODE_NAME) {
                        if(!findByValue(data2, &tok->prop.expr_list[x])) {
                            continue;
                        }
                    }
                }

                if(evaluate_postfix_expression(tok->prop.expr_list, tok->prop.expr_count)) {
                    copyToReturnResult(data2, return_value);
                }
            } ZEND_HASH_FOREACH_END();

            break;
    }
#endif
}

void iterateWildCard(zval * arr, struct token * tok, struct token * tok_last, zval * return_value)
{
#if PHP_MAJOR_VERSION < 7
    zval **data;
    HashPosition pos;
    zval *zv_dest;

    for(
        zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
        zend_hash_get_current_data_ex(HASH_OF(arr), (void**) &data, &pos) == SUCCESS;
        zend_hash_move_forward_ex(HASH_OF(arr), &pos)
    ) {
	if(tok == tok_last) {
		copyToReturnResult(data, return_value);
	} else if(Z_TYPE_P(*data) == IS_ARRAY){
		iterate(*data, (tok + 1), tok_last, return_value);
	}
    }
#else
    zval *data;
    zval *zv_dest;
    zend_string *key;
    ulong num_key;

    ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
	if(tok == tok_last) {
		copyToReturnResult(data, return_value);
	} else if(Z_TYPE_P(data) == IS_ARRAY){
		iterate(data, (tok + 1), tok_last, return_value);
	}
    } ZEND_HASH_FOREACH_END();
#endif
}

void deepJump(zval * arr, struct token * tok, struct token * tok_last, zval * return_value)
{
    if(arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
        return;
    }

    processChildKey(arr, tok, tok_last, return_value);

#if PHP_MAJOR_VERSION < 7
    zval **tmp;
    HashPosition pos;

    for(
        zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
        zend_hash_get_current_data_ex(HASH_OF(arr), (void**) &tmp, &pos) == SUCCESS;
        zend_hash_move_forward_ex(HASH_OF(arr), &pos)
    ) {
        deepJump(*tmp, tok, tok_last, return_value);
    }
#else
    zval *data;
    zval *zv_dest;
    zend_string *key;
    ulong num_key;

    ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
        deepJump(data, tok, tok_last, return_value);
    } ZEND_HASH_FOREACH_END();
#endif
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
bool findByValue(zval *arr, expr * node)
{
#if PHP_MAJOR_VERSION < 7
	zval ** data;

    int i;

    for(i = 0; i < node->label_count; i++) {
        if(zend_hash_find(HASH_OF(arr), node->label[i], strlen(node->label[i])+1, (void**)&data) == SUCCESS) {
            arr = *data;
        } else {
            node->value[0] = '\0';
            return false;
        }
    }

    if (Z_TYPE_P(*data) != IS_STRING) {
        convert_to_string(*data);
    }

    strcpy(node->value, Z_STRVAL_P(*data));

#else
    zval * data;

    int i;

    for(i = 0; i < node->label_count; i++) {

        if ((data = zend_hash_str_find(HASH_OF(arr), node->label[i], strlen(node->label[i]))) != NULL) {
            arr = data;
        } else {
            node->value[0] = '\0';
            return false;
        }
    }

    if (Z_TYPE_P(Z_REFVAL_P(data)) != IS_STRING) {
        convert_to_string(data);
    }

    strcpy(node->value, Z_STRVAL_P(data));
#endif
    return true;
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
bool checkIfKeyExists(zval *arr, expr * node)
{
#if PHP_MAJOR_VERSION < 7
	zval ** data;

	HashPosition pos;			/* hash iterator */

    int i;

    node->value_bool = false;

    //TODO clean this up?

    for(i = 0; i < node->label_count; i++) {
        if(zend_hash_find(HASH_OF(arr), node->label[i], strlen(node->label[i])+1, (void**)&data) != SUCCESS) {
            node->value_bool = false;
            return false;
        } else {
            node->value_bool = true;
            arr = *data;
        }
    }
#else
    zval * data;

    int i;

    node->value_bool = false;

    //TODO clean this up?

    for(i = 0; i < node->label_count; i++) {

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

bool isset2(expr * lh, expr * rh) {
    return (*lh).value_bool && (*rh).value_bool;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jsonpath)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
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
	PHP_FE(path_lookup, NULL)
	PHP_FE_END	/* Must be the last line in jsonpath_functions[] */
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
	PHP_RINIT(jsonpath),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(jsonpath),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(jsonpath),
	PHP_JSONPATH_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_JSONPATH
ZEND_GET_MODULE(jsonpath)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
