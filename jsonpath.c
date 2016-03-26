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

#include <stdbool.h>

/* If you declare any globals in php_jsonpath.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(jsonpath)
*/

/* True global resources - no need for thread safety here */
static int le_jsonpath;

void iterate(zval *arr, char * input_str, zval * return_value);
void deepSearch(zval * arr, char * field_name, zval * return_value);
void deepJump(char * field_name, zval * arr, char * p, zval * return_value);

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

void iterate(zval *arr, char * input_str, zval * return_value)
{
    struct token token_struct;
    char * save_ptr = input_str;
    struct token * token_ptr = &token_struct;
    zval **data, **data2;

    zval *newarr;
    MAKE_STD_ZVAL(newarr);
    array_init(newarr);

    zval *tmp;
    MAKE_STD_ZVAL(tmp);
    int x;
    HashPosition pos;

    zval *zv_dest;

    while(tokenize(&save_ptr, token_ptr)) {

        switch(token_struct.type) {
            case ROOT:
                break;
            case WILD_CARD:
                break;
            case DEEP_SCAN:
                if(*save_ptr == '\0') {
                    deepSearch(arr, token_struct.prop.val, return_value);
                } else {
                    deepJump(token_struct.prop.val, arr, save_ptr, return_value);
                }
                return;
            case CHILD_KEY:
                switch(token_struct.prop.type) {
                    case RANGE:
                        break;
                    case INDEX:

                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) == SUCCESS) {
                            for(x = 0; x < token_struct.prop.index_count; x++) {
                                if(zend_hash_index_find(HASH_OF(*data), token_struct.prop.indexes[x], (void**)&data2) == SUCCESS) {
                                    ALLOC_ZVAL(zv_dest);
                                    MAKE_COPY_ZVAL(data2, zv_dest);
                                    add_next_index_zval(newarr, zv_dest);
                                }
                            }
                        }

                        *data = newarr;

                        break;
                    case ANY:
                        break;
                    case SINGLE_KEY:
                        if(arr == NULL) {
                            return;
                        }
                        if(zend_hash_find(HASH_OF(arr), token_struct.prop.val, strlen(token_struct.prop.val) + 1, (void**)&data) != SUCCESS) {
                            return;
                        }
                        arr = *data;
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

void deepJump(char * field_name, zval * arr, char * save_ptr, zval * return_value)
{
    if(arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
        return;
    }

    zval *z_array, **data1;

    if(zend_hash_find(HASH_OF(arr), field_name, strlen(field_name)+1, (void**)&data1) == SUCCESS) {
        iterate(*data1, save_ptr, return_value);
    }

    HashPosition pos;
    zval **tmp;
    for(
        zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
        zend_hash_get_current_data_ex(HASH_OF(arr), (void**) &tmp, &pos) == SUCCESS;
        zend_hash_move_forward_ex(HASH_OF(arr), &pos)
    ) {
        deepJump(field_name, *tmp, save_ptr, return_value);
    }
}

void deepSearch(zval * arr, char * field_name, zval * return_value)
{
    if(arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
        return;
    }

    zval *z_array, **data;
    zval *tmp;
            
    if(zend_hash_find(HASH_OF(arr), field_name, strlen(field_name)+1, (void**)&data) == SUCCESS) {
        zval *newarr;
        MAKE_STD_ZVAL(newarr);
        array_init(newarr);
        MAKE_COPY_ZVAL(data, newarr);
        add_next_index_zval(return_value, newarr);
    }

    HashPosition pos;
    zval **atmp;

    for(
        zend_hash_internal_pointer_reset_ex(HASH_OF(arr), &pos);
        zend_hash_get_current_data_ex(HASH_OF(arr), (void**) &atmp, &pos) == SUCCESS;
        zend_hash_move_forward_ex(HASH_OF(arr), &pos)
    ) {
        deepSearch(*atmp, field_name, return_value);
    }
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
