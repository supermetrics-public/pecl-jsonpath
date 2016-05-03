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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "jsonpathtokenizer.h"
#include "php_jsonpath.h"
#include "zend_operators.h"
#include <stdbool.h>

/* If you declare any globals in php_jsonpath.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(jsonpath)
*/

/* True global resources - no need for thread safety here */
static int le_jsonpath;

void iterate(zval *arr, char * input_str, zval * return_value);
void deepJump(struct token * token_struct, zval * arr, char * save_ptr, zval * return_value);
void findByValue(char * key, zval *arr, char * str_ptr);

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("jsonpath.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_jsonpath_globals, jsonpath_globals)
    STD_PHP_INI_ENTRY("jsonpath.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_jsonpath_globals, jsonpath_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_jsonpath_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_jsonpath_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "jsonpath", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

PHP_FUNCTION(path_lookup)
{

    char * path;
    int path_len;
    zval *z_array;
    HashTable *arr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as", &z_array, &path, &path_len) == FAILURE) {
        return;
    }

//    zend_hash_internal_pointer_reset(arr);

    array_init(return_value);

    iterate(z_array, path, return_value);

    return;
//    RETVAL_ZVAL_FAST(*data);
//    RETURN_STRING(curBuffer, strlen(curBuffer));

//    RETURN_NULL();
//End
}

void output_postifx_expr(expr * expr, int count) {

    int i;

const char * const visible[] = {
    "==\0",
    "!=\0",
    "<\0",
    "<=\0",
    ">\0",
    ">=\0",
    "ISSET\0",
    "||\0",
    "&&\0",
    "(\0",
    ")\0",
    "<\0"
};


    for(i = 0; i < count; i++) {
        switch(get_token_type(expr[i].type)) {
            case TYPE_OPERATOR:
                printf("%d %s \n", i, visible[expr[i].type]);
                break;
            case TYPE_OPERAND:
            case TYPE_PAREN:
                printf("%d %s (%s) \n", i, expr[i].value, expr[i].label);
                break;
        }
    }
}

void iterate(zval *arr, char * input_str, zval * return_value)
{
    struct token token_struct;
    char * save_ptr = input_str;
    struct token * token_ptr = &token_struct;
    zval **data, **data2;
    zval *tmp;

    zval *newarr;
    MAKE_STD_ZVAL(newarr);
    array_init(newarr);

    zval *tmpz;
    MAKE_STD_ZVAL(tmp);
    int x;
    HashPosition pos;

    zval *zv_dest;

    MAKE_STD_ZVAL(tmpz);

    int i;

    while(tokenize(&save_ptr, token_ptr)) {

        switch(token_struct.type) {
            case ROOT:
                break;
            case WILD_CARD:

                if(*save_ptr == '\0') {
                    //TODO Throw some error
                }
                for(
                    zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
                    zend_hash_get_current_data_ex(HASH_OF(arr), (void**) &data, &pos) == SUCCESS;
                    zend_hash_move_forward_ex(HASH_OF(arr), &pos)
                ) {
                    ALLOC_ZVAL(zv_dest);
                    MAKE_COPY_ZVAL(data, zv_dest);
                    add_next_index_zval(return_value, zv_dest);
                }
                return;
            case DEEP_SCAN:
                deepJump(&token_struct, arr, save_ptr, return_value);
                return;
            case CHILD_KEY:
                switch(token_struct.prop.type) {
                    case RANGE:

                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
                            for(x = token_struct.prop.indexes[0]; x < token_struct.prop.indexes[1]; x++) {
                                if(zend_hash_index_find(HASH_OF(*data), x, (void**)&data2) == SUCCESS) {
                                    if(*save_ptr == '\0') {
                                        ALLOC_ZVAL(zv_dest);
                                        MAKE_COPY_ZVAL(data2, zv_dest);
                                        add_next_index_zval(return_value, zv_dest);
                                    } else {
                                        iterate(*data2, save_ptr, return_value);
                                    }
                                }
                            }
                        }

                        return;
                    case INDEX:

                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
                            for(x = 0; x < token_struct.prop.index_count; x++) {
                                if(zend_hash_index_find(HASH_OF(*data), token_struct.prop.indexes[x], (void**)&data2) == SUCCESS) {
                                    if(*save_ptr == '\0') {
                                        ALLOC_ZVAL(zv_dest);
                                        MAKE_COPY_ZVAL(data2, zv_dest);
                                        add_next_index_zval(return_value, zv_dest);
                                    } else {
                                        iterate(*data2, save_ptr, return_value);
                                    }
                                }
                            }
                        }
                        return;
                    case ANY:

                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
                            for(
                                zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
                                zend_hash_get_current_data_ex(HASH_OF(*data), (void**) &data2, &pos) == SUCCESS;
                                zend_hash_move_forward_ex(HASH_OF(*data), &pos)
                            ) {
                                if(*save_ptr == '\0') {
                                    ALLOC_ZVAL(zv_dest);
                                    MAKE_COPY_ZVAL(data2, zv_dest);
                                    add_next_index_zval(return_value, zv_dest);
                                } else {
                                    iterate(*data2, save_ptr, return_value);
                                }
                            }
                        }

                        return;
                    case SINGLE_KEY:
                        if(arr == NULL) {
                            return;
                        }
                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) != SUCCESS) {
                            return;
                        }
                        arr = *data;
                        break;
                    case FILTER:

//                        ZVAL_STRING(tmpz, token_struct.prop.expr.rh_val, 1);
                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
                            for(
                                zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
                                zend_hash_get_current_data_ex(HASH_OF(*data), (void**) &data2, &pos) == SUCCESS;
                                zend_hash_move_forward_ex(HASH_OF(*data), &pos)
                            ) {



                                // For each array entry, find the node names and populate their values
                                // Fill up expression NODE_NAME VALS
                                for(i = 0; i < token_struct.prop.expr_count; i++) {
                                    if(token_struct.prop.expr_list[i].type == NODE_NAME) {
                                        findByValue(token_struct.prop.expr_list[i].label, *data2, token_struct.prop.expr_list[i].value);
                                        printf("LAbel is %s", token_struct.prop.expr_list[i].value);
//                                        printf("BACK AT THE TOP count is %d label is %s %s\n", token_struct.prop.expr_count, token_struct.prop.expr_list[i].value, token_struct.prop.expr_list[i].label);
                                    }
                                }

                                printf("\nThe expr count is %d\n", token_struct.prop.expr_count);
                                output_postifx_expr(token_struct.prop.expr_list, token_struct.prop.expr_count);

                                if(evaluate_postfix_expression(token_struct.prop.expr_list, token_struct.prop.expr_count)) {
                                    ALLOC_ZVAL(zv_dest);
                                    MAKE_COPY_ZVAL(data2, zv_dest);
                                    add_next_index_zval(return_value, zv_dest);
                                } else {
                                    printf("Unsuccessful evaluation EXPR COUNT%d\n", token_struct.prop.expr_count);
                                }
                            }
                        }

//                        switch(token_struct.prop.expr.op) {
//
//                            case EQ:
//                                ZVAL_STRING(tmpz, token_struct.prop.expr.rh_val, 1);
//                                if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
//                                    for(
//                                        zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
//                                        zend_hash_get_current_data_ex(HASH_OF(*data), (void**) &data2, &pos) == SUCCESS;
//                                        zend_hash_move_forward_ex(HASH_OF(*data), &pos)
//                                    ) {
//
//                                        findByValue(token_struct.prop.expr.lh_val, tmpz, *data2, return_value);
//                                    }
//                                }
//                                break;
//                            case NE:
//                                printf("BOOPING EQ!");
//                                break;
//                            case LT:
//                                printf("CLOOPING EQ!");
//                                break;
//                            case LTE:
//                                ZVAL_STRING(tmpz, token_struct.prop.expr.rh_val, 1);
//
//
//                                if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
//                                    for(
//                                        zend_hash_internal_pointer_reset_ex(HASH_OF(*data), &pos);
//                                        zend_hash_get_current_data_ex(HASH_OF(*data), (void**) &data2, &pos) == SUCCESS;
//                                        zend_hash_move_forward_ex(HASH_OF(*data), &pos)
//                                    ) {
//
//                                        findByValue(token_struct.prop.expr.lh_val, tmpz, *data2, return_value);
//                                    }
//                                }
//
//                                break;
//                            case GT:
//                                printf("ELOOPING EQ!");
//                                break;
//                            case GTE:
//                                printf("FLOOPING EQ!");
//                                break;
//                            case ISSET:
//                                printf("GLOOPING EQ!");
//                                break;
//                        }

                        data = NULL;

                        break;
                }
                break;
        }
    }

    if(*save_ptr == '\0' && data != NULL) {

        zval *newarr;
        MAKE_STD_ZVAL(newarr);
        array_init(newarr);

        MAKE_COPY_ZVAL(data, newarr);

        add_next_index_zval(return_value, newarr);
    }
}

void deepJump(struct token * token_struct, zval * arr, char * save_ptr, zval * return_value)
{
    if(arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
        return;
    }

    zval *z_array, **data1, **data2, **data3;
    int x;

    HashPosition pos;

    zval *zv_dest;

    if(zend_hash_find(HASH_OF(arr), token_struct->prop.val, strlen(token_struct->prop.val)+1, (void**)&data1) == SUCCESS) {

        switch(token_struct->prop.type) {
            case RANGE:

                if(zend_hash_find(HASH_OF(arr), token_struct->prop.val, strlen(token_struct->prop.val) + 1, (void**)&data2) == SUCCESS) {
                    for(x = token_struct->prop.indexes[0]; x < token_struct->prop.indexes[1]; x++) {
                        if(zend_hash_index_find(HASH_OF(*data2), x, (void**)&data3) == SUCCESS) {
                            if(*save_ptr == '\0') {
                                ALLOC_ZVAL(zv_dest);
                                MAKE_COPY_ZVAL(data3, zv_dest);
                                add_next_index_zval(return_value, zv_dest);
                            } else {
                                iterate(*data3, save_ptr, return_value);
                            }
                        }
                    }
                }

                break;
            case INDEX:

                if(zend_hash_find(HASH_OF(arr), token_struct->prop.val, strlen(token_struct->prop.val) + 1, (void**)&data2) == SUCCESS) {
                    for(x = 0; x < token_struct->prop.index_count; x++) {
                        if(zend_hash_index_find(HASH_OF(*data2), token_struct->prop.indexes[x], (void**)&data3) == SUCCESS) {
                            if(*save_ptr == '\0') {
                                ALLOC_ZVAL(zv_dest);
                                MAKE_COPY_ZVAL(data3, zv_dest);
                                add_next_index_zval(return_value, zv_dest);
                            } else {
                                iterate(*data3, save_ptr, return_value);
                            }
                        }
                    }
                }
                break;
            case ANY:

                if(zend_hash_find(HASH_OF(arr), token_struct->prop.val, strlen(token_struct->prop.val) + 1, (void**)&data2) == SUCCESS) {
                    for(
                        zend_hash_internal_pointer_reset_ex(HASH_OF(*data2), &pos);
                        zend_hash_get_current_data_ex(HASH_OF(*data2), (void**) &data3, &pos) == SUCCESS;
                        zend_hash_move_forward_ex(HASH_OF(*data2), &pos)
                    ) {
                        if(*save_ptr == '\0') {
                            ALLOC_ZVAL(zv_dest);
                            MAKE_COPY_ZVAL(data3, zv_dest);
                            add_next_index_zval(return_value, zv_dest);
                        } else {
                            iterate(*data3, save_ptr, return_value);
                        }
                    }
                }

                break;
            case SINGLE_KEY:
            default:
                if(zend_hash_find(HASH_OF(arr), token_struct->prop.val, strlen(token_struct->prop.val) + 1, (void**)&data2) == SUCCESS) {
                    if(*save_ptr == '\0') {
                        ALLOC_ZVAL(zv_dest);
                        MAKE_COPY_ZVAL(data2, zv_dest);
                        add_next_index_zval(return_value, zv_dest);
                    } else {
                        iterate(*data2, save_ptr, return_value);
                    }
                }

                break;
        }
    }

    zval **tmp;
    for(
        zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
        zend_hash_get_current_data_ex(HASH_OF(arr), (void**) &tmp, &pos) == SUCCESS;
        zend_hash_move_forward_ex(HASH_OF(arr), &pos)
    ) {
//        if(*save_ptr == '\0') {
            deepJump(token_struct, *tmp, save_ptr, return_value);
//        }
    }
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
void findByValue(char * key, zval *arr, char * str_ptr)
{
	zval *entry,				/* pointer to array entry */
        * res,					/* comparison result */
        ** data;

    zval *zv_dest;

	HashPosition pos;			/* hash iterator */

    MAKE_STD_ZVAL(res);
    MAKE_STD_ZVAL(entry);

    zval *var;
    char *cstr;
    int cstrlen;

    //TODO key is not a trailing \0
    if(zend_hash_find(HASH_OF(arr), key, strlen(key) + 1, (void**)&data) == SUCCESS) {

        if (Z_TYPE_P(*data) != IS_STRING) {
            convert_to_string(*data);
        }
//printf("FOUND!");
        strcpy(str_ptr, Z_STRVAL_P(*data));
        printf("ddd %s!", Z_STRVAL_P(*data));
//        cstr =
//        cstrlen = Z_STRLEN_P(var);
    } else {
//        printf("NOT FOUND!");
    }


//    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&entry, &pos) == SUCCESS) {
////
//		compare_function(&res, value, *entry TSRMLS_CC);
////
//		if (Z_LVAL(res)) {
//            zend_hash_get_current_key_zval_ex(Z_ARRVAL_P(array), entry, &pos);
//////
////            ALLOC_ZVAL(zv_dest);
////            MAKE_COPY_ZVAL(&data, zv_dest);
////            add_next_index_zval(return_value, zv_dest);
//		}
////
//		zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
//    }
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

    zval *a, *b, *result;
    MAKE_STD_ZVAL(a);
    MAKE_STD_ZVAL(b);
    MAKE_STD_ZVAL(result);

    ZVAL_STRING(a, (*lh).value, 0);
    ZVAL_STRING(b, (*rh).value, 0);

    compare_function(result, a, b TSRMLS_CC);

    return Z_LVAL_P(result) == 0;
}

/* {{{ php_jsonpath_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_jsonpath_init_globals(zend_jsonpath_globals *jsonpath_globals)
{
	jsonpath_globals->global_value = 0;
	jsonpath_globals->global_string = NULL;
}
*/
/* }}} */

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
	PHP_FE(confirm_jsonpath_compiled,	NULL)		/* For testing, remove later. */
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
