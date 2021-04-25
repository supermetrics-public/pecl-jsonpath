
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ext/spl/spl_exceptions.h>

#include "ext/standard/info.h"
#include "php.h"
#include "php_ini.h"
#include "php_jsonpath.h"
#include "src/jsonpath/interpreter.h"
#include "src/jsonpath/lexer.h"
#include "src/jsonpath/parser.h"
#include "zend_exceptions.h"

/* True global resources - no need for thread safety here */
static int le_jsonpath;
bool scanTokens(char* json_path, lex_token tok[], char tok_literals[][PARSE_BUF_LEN], int* tok_count);
#ifdef JSONPATH_DEBUG
void print_lex_tokens(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_literals[][PARSE_BUF_LEN], int lex_tok_count,
                      const char* m);
#endif

zend_class_entry* jsonpath_ce;

#if PHP_VERSION_ID < 80000
#include "jsonpath_legacy_arginfo.h"
#else
#include "jsonpath_arginfo.h"
#endif

PHP_METHOD(JsonPath, find) {
  /* parse php method parameters */

  char* j_path;
  size_t j_path_len;
  zval* search_target;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "as", &search_target, &j_path, &j_path_len) == FAILURE) {
    return;
  }

  /* tokenize JSON-path string */

  lex_token lex_tok[PARSE_BUF_LEN];
  char lex_tok_literals[PARSE_BUF_LEN][PARSE_BUF_LEN];
  int lex_tok_count = 0;

  if (!scanTokens(j_path, lex_tok, lex_tok_literals, &lex_tok_count)) {
    return;
  }

  if (!sanity_check(lex_tok, lex_tok_count)) {
    return;
  }

#ifdef JSONPATH_DEBUG
  print_lex_tokens(lex_tok, lex_tok_literals, lex_tok_count, "Lexer - Processed tokens");
#endif

  /* assemble an array of query execution instructions from parsed tokens */

  struct ast_node head;
  int i = 0;

  if (!build_parse_tree(lex_tok, lex_tok_literals, &i, lex_tok_count, &head)) {
    free_ast_nodes(head.next);
    return;
  }

  if (!validate_parse_tree(head.next)) {
    free_ast_nodes(head.next);
    return;
  }

#ifdef JSONPATH_DEBUG
  print_ast(head.next, "Parser - AST sent to interpreter", 0);
#endif

  /* execute the JSON-path query instructions against the search target (PHP object/array) */

  array_init(return_value);

  eval_ast(search_target, search_target, head.next, return_value);

  free_ast_nodes(head.next);

  /* return false if no results were found by the JSON-path query */

  if (zend_hash_num_elements(HASH_OF(return_value)) == 0) {
    convert_to_boolean(return_value);
    RETURN_FALSE;
  }
}

bool scanTokens(char* json_path, lex_token tok[], char tok_literals[][PARSE_BUF_LEN], int* tok_count) {
  lex_token cur_tok;
  char* p = json_path;
  char buffer[PARSE_BUF_LEN];

  int i = 0;

  while ((cur_tok = scan(&p, buffer, sizeof(buffer), json_path)) != LEX_NOT_FOUND) {
    if (i >= PARSE_BUF_LEN) {
      zend_throw_exception(spl_ce_RuntimeException, "The query is too long. Token count exceeds PARSE_BUF_LEN.", 0);
      return false;
    }

    switch (cur_tok) {
      case LEX_NODE:
      case LEX_LITERAL:
      case LEX_LITERAL_BOOL:
        strcpy(tok_literals[i], buffer);
        break;
      case LEX_ERR:
        return false;
      default:
        tok_literals[i][0] = '\0';
        break;
    }

    tok[i] = cur_tok;
    i++;
  }

  *tok_count = i;

  return true;
}

#ifdef JSONPATH_DEBUG
void print_lex_tokens(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_literals[][PARSE_BUF_LEN], int lex_tok_count,
                      const char* m) {
  printf("--------------------------------------\n");
  printf("%s\n\n", m);

  for (int i = 0; i < lex_tok_count; i++) {
    printf("\t• %s", LEX_STR[lex_tok[i]]);
    if (strlen(lex_tok_literals[i]) > 0) {
      printf(" [val=%s]", lex_tok_literals[i]);
    }
    printf("\n");
  }
}
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jsonpath) {
  zend_class_entry jsonpath_class_entry;
  INIT_CLASS_ENTRY(jsonpath_class_entry, "JsonPath", class_JsonPath_methods);

  jsonpath_ce = zend_register_internal_class(&jsonpath_class_entry);

  return SUCCESS;
}

/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(jsonpath) {
  /* uncomment this line if you have INI entries
     UNREGISTER_INI_ENTRIES();
   */
  return SUCCESS;
}

/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(jsonpath) { return SUCCESS; }

/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(jsonpath) { return SUCCESS; }

/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(jsonpath) {
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
    PHP_FE_END /* Must be the last line in jsonpath_functions[] */
};

/* }}} */

/* {{{ jsonpath_module_entry
 */
zend_module_entry jsonpath_module_entry = {
    STANDARD_MODULE_HEADER,  "jsonpath",           jsonpath_functions,        PHP_MINIT(jsonpath),
    PHP_MSHUTDOWN(jsonpath), PHP_RINIT(jsonpath), /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(jsonpath),                      /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(jsonpath),     PHP_JSONPATH_VERSION, STANDARD_MODULE_PROPERTIES};

/* }}} */

#ifdef COMPILE_DL_JSONPATH
ZEND_GET_MODULE(jsonpath)
#endif
