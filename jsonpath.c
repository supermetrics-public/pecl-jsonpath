
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
void iterate(zval* arr, operator * tok, operator * tok_last, zval* return_value);
void recurse(zval* arr, operator * tok, operator * tok_last, zval* return_value);
bool findByValue(zval* arr, expr_operator* node);
bool checkIfKeyExists(zval* arr, expr_operator* node);
void processChildKey(zval* arr, operator * tok, operator * tok_last, zval* return_value);
void iterateWildCard(zval* arr, operator * tok, operator * tok_last, zval* return_value);
bool is_scalar(zval* arg);

zend_class_entry* jsonpath_ce;

#if PHP_VERSION_ID < 80000
#include "jsonpath_legacy_arginfo.h"
#else
#include "jsonpath_arginfo.h"
#endif

PHP_METHOD(JsonPath, find)
{
    char* path;
    size_t path_len;
    zval* z_array;
    HashTable* arr;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "as", &z_array, &path, &path_len) == FAILURE) {
        return;
    }

    array_init(return_value);

    lex_token lex_tok[PARSE_BUF_LEN];
    char lex_tok_values[PARSE_BUF_LEN][PARSE_BUF_LEN];
    int lex_tok_count = 0;

    char* p = path;

    char buffer[PARSE_BUF_LEN];

    lex_token* ptr = lex_tok;

    lex_error err;

    while ((*ptr = scan(&p, buffer, sizeof(buffer), &err)) != LEX_NOT_FOUND) {
        switch (*ptr) {
        case LEX_NODE:
        case LEX_LITERAL:
        case LEX_LITERAL_BOOL:
            strcpy(lex_tok_values[lex_tok_count], buffer);
            break;
        case LEX_ERR:
            snprintf(err.msg, sizeof(err.msg), "%s at position %ld", err.msg, (err.pos - path));
            zend_throw_exception(spl_ce_RuntimeException, err.msg, 0);
            return;
        default:
            lex_tok_values[lex_tok_count][0] = '\0';
            break;
        }

        ptr++;

        lex_tok_count++;
    }

    operator tok[100];
    int tok_count = 0;
    int* int_ptr = &tok_count;

    parse_error p_err;

    if (!build_parse_tree(lex_tok, lex_tok_values, lex_tok_count, tok, int_ptr, &p_err)) {
        zend_throw_exception(spl_ce_RuntimeException, p_err.msg, 0);
    }

    operator * tok_ptr_start;
    operator * tok_ptr_end;

    tok_ptr_start = &tok[0];
    tok_ptr_end = &tok[tok_count - 1];

    iterate(z_array, tok_ptr_start, tok_ptr_end, return_value);

    operator * fr = tok_ptr_start;

    while (fr <= tok_ptr_end) {
        if (fr->filter_type == FLTR_EXPR) {
            efree((void*)fr->expressions);
        }
        fr++;
    }

    if (zend_hash_num_elements(HASH_OF(return_value)) == 0) {
        convert_to_boolean(return_value);
        RETURN_FALSE;
    }

    return;
}

void iterate(zval* arr, operator * tok, operator * tok_last, zval* return_value)
{
    if (tok > tok_last) {
        return;
    }

    switch (tok->type) {
    case ROOT:
        if (tok->filter_type == FLTR_RANGE) {
            processChildKey(arr, tok, tok_last, return_value);
        }
        else {
            iterate(arr, (tok + 1), tok_last, return_value);
        }
        break;
    case WILD_CARD:
        iterateWildCard(arr, tok, tok_last, return_value);
        return;
    case DEEP_SCAN:
        recurse(arr, tok, tok_last, return_value);
        return;
    case CHILD_KEY:
        processChildKey(arr, tok, tok_last, return_value);
        return;
    }
}

void copyToReturnResult(zval* arr, zval* return_value)
{
    zval tmp;
    ZVAL_COPY_VALUE(&tmp, arr);
    zval_copy_ctor(&tmp);
    add_next_index_zval(return_value, &tmp);
}

void processChildKey(zval* arr, operator * tok, operator * tok_last, zval* return_value)
{
    zval* data, * data2;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return;
    }

    // FLTR_WILD_CARD doesn't necessarily target a specific node. E.g. $..[*] should loop through the
    // whole array.
    // TODO: Check if we need to deal with empty string as array key
    if (tok->node_value_len == 0) {
        data = arr;
    }
    else if ((data = zend_hash_str_find(HASH_OF(arr), tok->node_value, tok->node_value_len)) == NULL) {
        return;
    }

    int x;
    zend_string* key;
    zend_ulong num_key;
    int range_start = 0;
    int range_end = 0;
    int range_step = 1;
    expr_operator* node;

    switch (tok->filter_type) {
    case FLTR_RANGE:
        // [a:]
        if (tok->index_count == 1) {
            range_start = tok->indexes[0];
        }
        // [a:b], [a:b:]
        else if (tok->index_count == 2) {
            range_start = tok->indexes[0];
            range_end = tok->indexes[1];
        }
        // [a:b:c]
        else if (tok->index_count == 3) {
            range_start = tok->indexes[0];
            range_end = tok->indexes[1];
            if (tok->indexes[2] != 0) {
                range_step = tok->indexes[2];
            }
        }

        if (range_start < 0) {
            range_start = zend_hash_num_elements(HASH_OF(data)) - abs(range_start);
        }

        if (range_end <= 0) {
            range_end = zend_hash_num_elements(HASH_OF(data)) - abs(range_end);
        }

        for (x = range_start; x < range_end; x += range_step) {
            if ((data2 = zend_hash_index_find(HASH_OF(data), x)) != NULL) {
                if (tok == tok_last) {
                    copyToReturnResult(data2, return_value);
                }
                else {
                    iterate(data2, (tok + 1), tok_last, return_value);
                }
            }
        }
        return;
    case FLTR_INDEX:
        for (x = 0; x < tok->index_count; x++) {
            if (tok->indexes[x] < 0) {
                tok->indexes[x] = zend_hash_num_elements(HASH_OF(data)) - abs(tok->indexes[x]);
            }
            if ((data2 = zend_hash_index_find(HASH_OF(data), tok->indexes[x])) != NULL) {
                if (tok == tok_last) {
                    copyToReturnResult(data2, return_value);
                }
                else {
                    iterate(data2, (tok + 1), tok_last, return_value);
                }
            }
        }
        return;
    case FLTR_WILD_CARD:

        ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(data), num_key, key, data2) {
            if (tok == tok_last) {
                copyToReturnResult(data2, return_value);
            }
            else {
                iterate(data2, (tok + 1), tok_last, return_value);
            }
        }
        ZEND_HASH_FOREACH_END();

        return;
    case FLTR_NODE:
        if (tok == tok_last) {
            copyToReturnResult(data, return_value);
        }
        else {
            iterate(data, (tok + 1), tok_last, return_value);
        }
        return;
    case FLTR_EXPR:

        ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(data), num_key, key, data2) {
            // For each array entry, find the node names and populate their values
            // Fill up expression NODE_NAME VALS
            for (x = 0; x < tok->expression_count; x++) {
                if (x < tok->expression_count - 1 && tok->expressions[x + 1].type == EXPR_ISSET) {
                    if (!checkIfKeyExists(data2, &tok->expressions[x])) {
                        continue;
                    }
                }
                else if (tok->expressions[x].type == EXPR_NODE_NAME) {
                    if (!findByValue(data2, &tok->expressions[x])) {
                        continue;
                    }
                }
            }

            if (evaluate_postfix_expression(tok->expressions, tok->expression_count)) {
                if (tok == tok_last) {
                    copyToReturnResult(data2, return_value);
                }
                else {
                    iterate(data2, (tok + 1), tok_last, return_value);
                }
            }

            // Clean up node values to prevent incorrect node values during recursive wildcard iterations
            for (x = 0; x < tok->expression_count; x++) {
                if (tok->expressions[x].type == EXPR_NODE_NAME) {
                    node = &tok->expressions[x];
                    node->value[0] = '\0';
                }
            }
        }
        ZEND_HASH_FOREACH_END();

        break;
    }
}

void iterateWildCard(zval* arr, operator * tok, operator * tok_last, zval* return_value)
{
    zval* data;
    zval* zv_dest;
    zend_string* key;
    zend_ulong num_key;

    ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
        if (tok == tok_last) {
            copyToReturnResult(data, return_value);
        }
        else if (Z_TYPE_P(data) == IS_ARRAY) {
            iterate(data, (tok + 1), tok_last, return_value);
        }
    }
    ZEND_HASH_FOREACH_END();
}

void recurse(zval* arr, operator * tok, operator * tok_last, zval* return_value)
{
    if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
        return;
    }

    processChildKey(arr, tok, tok_last, return_value);

    zval* data;
    zval* zv_dest;
    zend_string* key;
    zend_ulong num_key;

    ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
        recurse(data, tok, tok_last, return_value);
    }
    ZEND_HASH_FOREACH_END();
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
bool findByValue(zval* arr, expr_operator* node)
{
    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return false;
    }

    zval* data;

    int i;

    for (i = 0; i < node->label_count; i++) {

        if ((data = zend_hash_str_find(HASH_OF(arr), node->label[i], strlen(node->label[i]))) != NULL) {
            arr = data;
        }
        else {
            node->value[0] = '\0';
            return false;
        }
    }

    if (!is_scalar(data)) {
        return false;
    }

    char* s = NULL;
    size_t s_len;

    if (Z_TYPE_P(data) == IS_TRUE) {
        strncpy(node->value, "JP_LITERAL_TRUE", 15);
        node->value[15] = '\0';
    }
    else if (Z_TYPE_P(data) == IS_FALSE) {
        strncpy(node->value, "JP_LITERAL_FALSE", 16);
        node->value[16] = '\0';
    }
    else if (Z_TYPE_P(data) != IS_STRING) {

        zval zcopy;
        int free_zcopy;
        char* s = NULL;
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

    }
    else {
        s_len = Z_STRLEN_P(data);
        s = Z_STRVAL_P(data);
        strncpy(node->value, s, s_len);
        node->value[s_len] = '\0';
    }

    return true;
}

/**
 * *value   value to check for
 * *array   array to check in
 * **entry  pointer to array entry
 */
bool checkIfKeyExists(zval* arr, expr_operator* node)
{
    zval* data;

    int i;

    node->value_bool = false;

    for (i = 0; i < node->label_count; i++) {

        if ((data = zend_hash_str_find(HASH_OF(arr), node->label[i], strlen(node->label[i]))) == NULL) {
            node->value_bool = false;
            return false;
        }
        else {
            node->value_bool = true;
            arr = data;
        }
    }

    return true;
}

bool compare_lt(expr_operator* lh, expr_operator* rh)
{
    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    compare_function(&result, &a, &b);
    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);

    bool res = (Z_LVAL(result) < 0);

    return res;
}

bool compare_gt(expr_operator* lh, expr_operator* rh)
{
    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    compare_function(&result, &a, &b);
    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);

    bool res = (Z_LVAL(result) > 0);

    return res;
}

bool compare_lte(expr_operator* lh, expr_operator* rh)
{
    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    compare_function(&result, &a, &b);
    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);

    bool res = (Z_LVAL(result) <= 0);

    return res;
}

bool compare_gte(expr_operator* lh, expr_operator* rh)
{
    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    compare_function(&result, &a, &b);
    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);

    bool res = (Z_LVAL(result) >= 0);

    return res;
}

bool compare_and(expr_operator* lh, expr_operator* rh)
{
    return (*lh).value_bool && (*rh).value_bool;
}

bool compare_or(expr_operator* lh, expr_operator* rh)
{
    return (*lh).value_bool || (*rh).value_bool;
}

bool compare_eq(expr_operator* lh, expr_operator* rh)
{
    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    compare_function(&result, &a, &b);
    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);

    bool res = (Z_LVAL(result) == 0);

    return res;
}

bool compare_neq(expr_operator* lh, expr_operator* rh)
{
    zval a, b, result;

    ZVAL_STRING(&a, (*lh).value);
    ZVAL_STRING(&b, (*rh).value);

    compare_function(&result, &a, &b);
    zval_ptr_dtor(&a);
    zval_ptr_dtor(&b);

    bool res = (Z_LVAL(result) != 0);

    return res;
}

bool compare_isset(expr_operator* lh, expr_operator* rh)
{
    return (*lh).value_bool && (*rh).value_bool;
}

bool compare_rgxp(expr_operator* lh, expr_operator* rh)
{
    zval pattern;
    pcre_cache_entry* pce;

    ZVAL_STRING(&pattern, (*rh).value);

    if ((pce = pcre_get_compiled_regex_cache(Z_STR(pattern))) == NULL) {
        zval_ptr_dtor(&pattern);
        return false;
    }

    zval retval;
    zval subpats;

    ZVAL_NULL(&retval);
    ZVAL_NULL(&subpats);

    zend_string* s_lh = zend_string_init((*lh).value, strlen((*lh).value), 0);

    php_pcre_match_impl(pce, s_lh, &retval, &subpats, 0, 0, 0, 0);

    zend_string_release_ex(s_lh, 0);
    zval_ptr_dtor(&subpats);
    zval_ptr_dtor(&pattern);

    return Z_LVAL(retval) > 0;
}

bool is_scalar(zval* arg)
{
    switch (Z_TYPE_P(arg)) {
    case IS_FALSE:
    case IS_TRUE:
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

void* jpath_malloc(size_t size) {
    return emalloc(size);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jsonpath)
{
    zend_class_entry jsonpath_class_entry;
    INIT_CLASS_ENTRY(jsonpath_class_entry, "JsonPath", class_JsonPath_methods);

    jsonpath_ce = zend_register_internal_class(&jsonpath_class_entry);

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
    php_info_print_table_row(2, "jsonpath support", "enabled");
    php_info_print_table_row(2, "jsonpath version", PHP_JSONPATH_VERSION);
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
