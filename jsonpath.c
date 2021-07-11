
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ext/spl/spl_exceptions.h"
#include "ext/standard/info.h"
#include "php.h"
#include "php_jsonpath.h"
#include "src/jsonpath/exceptions.h"
#include "src/jsonpath/interpreter.h"
#include "src/jsonpath/lexer.h"
#include "src/jsonpath/parser.h"
#include "zend_exceptions.h"

static bool scan_tokens(char* json_path, struct jpath_token* tok, int* tok_count);
#ifdef JSONPATH_DEBUG
void print_lex_tokens(struct jpath_token lex_tok[], int lex_tok_count, const char* m);
#endif

zend_class_entry* jsonpath_ce;
zend_class_entry* jsonpath_exception_ce;

#if PHP_VERSION_ID < 80000
#include "jsonpath_legacy_arginfo.h"
#else
#include "jsonpath_arginfo.h"
#endif

#define LEX_TOK_ARR_LEN 64

PHP_METHOD(JsonPath_JsonPath, find) {
  char* j_path;
  size_t j_path_len;
  zval* search_target;

  /* parse php method parameters */
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "as", &search_target, &j_path, &j_path_len) == FAILURE) {
    return;
  }

  /* Keep the original parameter untouched for the stack trace and instead work with a copy that might be modified
   * during processing, e.g. due to escaped quotes in string literals */
  char* j_path_work_copy = estrdup(j_path);

  /* tokenize JSON-path string */

  struct jpath_token lex_tok[LEX_TOK_ARR_LEN];
  int lex_tok_count = 0;

  bool scan_ok = scan_tokens(j_path_work_copy, lex_tok, &lex_tok_count);

  if (!scan_ok) {
    efree(j_path_work_copy);
    return;
  }

#ifdef JSONPATH_DEBUG
  print_lex_tokens(lex_tok, lex_tok_count, "Lexer - Processed tokens");
#endif

  /* assemble an array of query execution instructions from parsed tokens */

  int i = 0;
  struct node_pool pool = {0};
  struct ast_node* head = parse_jsonpath(lex_tok, &i, lex_tok_count, &pool);

  if (head == NULL) {
    efree(j_path_work_copy);
    free_php_objects(&pool);
    return;
  }

#ifdef JSONPATH_DEBUG
  print_ast(head->next, "Parser - AST sent to interpreter", 0);
#endif

  /* execute the JSON-path query instructions against the search target (PHP object/array) */

  array_init(return_value);

  eval_ast(search_target, search_target, head, return_value);

  efree(j_path_work_copy);
  free_php_objects(&pool);

  /* return false if no results were found by the JSON-path query */

  if (zend_hash_num_elements(HASH_OF(return_value)) == 0) {
    convert_to_boolean(return_value);
    RETURN_FALSE;
  }
}

static bool scan_tokens(char* json_path, struct jpath_token* tok, int* tok_count) {
  char* p = json_path;
  int i = 0;

  while (*p != '\0') {
    if (i >= LEX_TOK_ARR_LEN) {
      throw_jsonpath_exception("The query is too long, token count exceeds maximum amount (%d)", LEX_TOK_ARR_LEN);
      return false;
    }

    if (!scan(&p, &tok[i], json_path)) {
      return false;
    }

    i++;
  }

  *tok_count = i;

  return true;
}

#ifdef JSONPATH_DEBUG
void print_lex_tokens(struct jpath_token lex_tok[], int lex_tok_count, const char* m) {
  printf("--------------------------------------\n");
  printf("%s\n\n", m);

  for (int i = 0; i < lex_tok_count; i++) {
    printf("\t• %s", LEX_STR[lex_tok[i].type]);
    if (lex_tok[i].len > 0) {
      printf(" [val=%.*s]", lex_tok[i].len, lex_tok[i].val);
    }
    printf("\n");
  }
}
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jsonpath) {
  jsonpath_ce = register_class_JsonPath_JsonPath();

  jsonpath_exception_ce = register_class_JsonPath_JsonPathException(spl_ce_RuntimeException);

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(jsonpath) {
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(jsonpath) {
  php_info_print_table_start();
  php_info_print_table_row(2, "jsonpath support", "enabled");
  php_info_print_table_row(2, "jsonpath version", PHP_JSONPATH_VERSION);
  php_info_print_table_end();
}
/* }}} */

/* {{{ jsonpath_functions[]
 *
 * Every user visible function must have an entry in jsonpath_functions[].
 */
const zend_function_entry jsonpath_functions[] = {
    PHP_FE_END /* Must be the last line in jsonpath_functions[] */
};
/* }}} */

/* {{{ jsonpath_module_entry
 */
zend_module_entry jsonpath_module_entry = {
    STANDARD_MODULE_HEADER,    /**/
    "jsonpath",                /**/
    jsonpath_functions,        /**/
    PHP_MINIT(jsonpath),       /**/
    PHP_MSHUTDOWN(jsonpath),   /**/
    NULL,                      /* nothing to do at request start */
    NULL,                      /* nothing to do at request end */
    PHP_MINFO(jsonpath),       /**/
    PHP_JSONPATH_VERSION,      /**/
    STANDARD_MODULE_PROPERTIES /**/
};
/* }}} */

#ifdef COMPILE_DL_JSONPATH
ZEND_GET_MODULE(jsonpath)
#endif