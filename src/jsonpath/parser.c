#include "parser.h"

#include <limits.h>

#include "exceptions.h"
#include "php.h"

#define CUR_POS() *lex_idx
#define CONSUME_TOKEN() (CUR_POS())++
#define CUR_TOKEN_LEN() lex_tok[CUR_POS()].len
#define CUR_TOKEN_LITERAL() lex_tok[CUR_POS()].val
#define CUR_TOKEN() lex_tok[CUR_POS()].type
#define GET_NODE(type) get_node_from_pool(pool, type)
#define GET_BINARY_NODE(type, left, right) get_binary_node_from_pool(pool, type, left, right)
#define HAS_TOKEN() (CUR_POS() < lex_tok_count)
#define PARSER_ARGS lex_tok, lex_idx, lex_tok_count, pool
#define PARSER_PARAMS struct jpath_token lex_tok[], int *lex_idx, int lex_tok_count, struct node_pool *pool
#define RETURN_IF_NULL(param) \
  if (param == NULL) return NULL

static bool is_logical_operator(lex_token type);
static bool make_numeric_node(struct ast_node* tok, char* str, int str_len);
static bool numeric_to_long(char* str, int str_len, long* dest);
static bool parse_filter_list(PARSER_PARAMS, struct ast_node* tok);
static bool validate_expression_head(struct ast_node* tok);
static struct ast_node* get_binary_node_from_pool(struct node_pool* pool, enum ast_type type, struct ast_node* left,
                                                  struct ast_node* right);
static struct ast_node* get_node_from_pool(struct node_pool* pool, enum ast_type type);
static struct ast_node* parse_and(PARSER_PARAMS);
static struct ast_node* parse_comparison(PARSER_PARAMS);
static struct ast_node* parse_equality(PARSER_PARAMS);
static struct ast_node* parse_expression(PARSER_PARAMS);
static struct ast_node* parse_filter(PARSER_PARAMS);
static struct ast_node* parse_operator(PARSER_PARAMS);
static struct ast_node* parse_or(PARSER_PARAMS);
static struct ast_node* parse_primary(PARSER_PARAMS);
static struct ast_node* parse_unary(PARSER_PARAMS);
static void ht_append_long(HashTable* ht, long val);
static void ht_append_string(HashTable* ht, char* str, int len);

const char* AST_STR[] = {
    "AST_AND",         /**/
    "AST_BOOL",        /**/
    "AST_CUR_NODE",    /**/
    "AST_DOUBLE",      /**/
    "AST_EQ",          /**/
    "AST_EXPR",        /**/
    "AST_GT",          /**/
    "AST_GTE",         /**/
    "AST_INDEX_LIST",  /**/
    "AST_INDEX_SLICE", /**/
    "AST_LITERAL",     /**/
    "AST_LONG",        /**/
    "AST_LT",          /**/
    "AST_LTE",         /**/
    "AST_NE",          /**/
    "AST_NEGATION",    /**/
    "AST_NODE_LIST",   /**/
    "AST_NULL",        /**/
    "AST_OR",          /**/
    "AST_RECURSE",     /**/
    "AST_RGXP",        /**/
    "AST_ROOT",        /**/
    "AST_SELECTOR",    /**/
    "AST_WILD_CARD",   /**/
};

bool is_unary(enum ast_type type) {
  return type == AST_NEGATION;
}

static struct ast_node* get_binary_node_from_pool(struct node_pool* pool, enum ast_type type, struct ast_node* left,
                                                  struct ast_node* right) {
  struct ast_node* node = get_node_from_pool(pool, type);
  if (node == NULL) {
    return NULL;
  }
  node->data.d_binary.left = left;
  node->data.d_binary.right = right;
  return node;
}

static struct ast_node* get_node_from_pool(struct node_pool* pool, enum ast_type type) {
  if (pool->cur_index >= NODE_POOL_LEN) {
    throw_jsonpath_exception(
        "Expression requires more parser node slots than are available (%d), try a shorter expression", NODE_POOL_LEN);
    return NULL;
  }
  struct ast_node* node = &pool->nodes[pool->cur_index++];
  node->type = type;
  return node;
}

static void ht_append_long(HashTable* ht, long val) {
  zval tmp;
  ZVAL_ARR(&tmp, ht);
  add_index_long(&tmp, zend_hash_num_elements(ht), val);
}

static void ht_append_string(HashTable* ht, char* str, int len) {
  zval tmp;
  ZVAL_ARR(&tmp, ht);
  add_index_stringl(&tmp, zend_hash_num_elements(ht), str, len);
}

static bool parse_filter_list(PARSER_PARAMS, struct ast_node* tok) {
  int slice_count = 0;

  /* assume filter type is an index list by default. this resolves type ambiguity of a filter containing no separators.
   * example: treat level4[0] as an index filter, not a slice. */
  tok->type = AST_INDEX_LIST;
  tok->data.d_list.ht = zend_new_array(1);
  /* used to determine if different separator types are present, default value is arbitrary */
  enum ast_type sep_found = AST_AND;

  for (; *lex_idx < lex_tok_count; (*lex_idx)++) {
    if (CUR_TOKEN() == LEX_EXPR_END) {
      /* lex_idx must point to LEX_EXPR_END after function returns */
      (*lex_idx)--;
      break;
    } else if (CUR_TOKEN() == LEX_CHILD_SEP) {
      if (sep_found == AST_INDEX_SLICE) {
        throw_jsonpath_exception("Multiple filter list separators `,` and `:` found, only one type is allowed");
        return false;
      }
      tok->type = sep_found = AST_INDEX_LIST;
    } else if (CUR_TOKEN() == LEX_SLICE) {
      if (sep_found == AST_INDEX_LIST) {
        throw_jsonpath_exception("Multiple filter list separators `,` and `:` found, only one type is allowed");
        return false;
      }

      tok->type = sep_found = AST_INDEX_SLICE;

      slice_count++;
      /* [:a] => [0:a] */
      /* [a::] => [a:0:] */
      if (slice_count > zend_hash_num_elements(tok->data.d_list.ht)) {
        if (slice_count == 1 || slice_count == 2) {
          ht_append_long(tok->data.d_list.ht, INT_MAX);
        }
      }
    } else if (CUR_TOKEN() == LEX_LITERAL_NUMERIC) {
      long idx = 0;

      if (!numeric_to_long(CUR_TOKEN_LITERAL(), CUR_TOKEN_LEN(), &idx)) {
        throw_jsonpath_exception("Unable to parse filter index value");
        return false;
      }
      ht_append_long(tok->data.d_list.ht, idx);
    } else if (CUR_TOKEN() == LEX_LITERAL) {
      if (sep_found == AST_INDEX_SLICE) {
        throw_jsonpath_exception("Array slice indexes must be integers");
        return false;
      }
      tok->type = sep_found = AST_NODE_LIST;
      ht_append_string(tok->data.d_list.ht, CUR_TOKEN_LITERAL(), CUR_TOKEN_LEN());
    } else {
      throw_jsonpath_exception("Unexpected token `%s` in filter", LEX_STR[CUR_TOKEN()]);
      return false;
    }
  }
  return true;
}

struct ast_node* parse_jsonpath(PARSER_PARAMS) {
  if (!HAS_TOKEN() || CUR_TOKEN() != LEX_ROOT) {
    throw_jsonpath_exception("JSONPath must start with a root operator `$`");
    return NULL;
  }

  CONSUME_TOKEN();

  struct ast_node* expr = NULL;

  if (HAS_TOKEN()) {
    expr = parse_operator(PARSER_ARGS);
    if (expr == NULL) {
      return NULL;
    }
  }

  struct ast_node* ret = GET_NODE(AST_ROOT);
  RETURN_IF_NULL(ret);
  ret->next = expr;

  return ret;
}

static struct ast_node* parse_operator(PARSER_PARAMS) {
  if (!HAS_TOKEN()) {
    return NULL;
  }

  struct ast_node* expr = NULL;

  switch (CUR_TOKEN()) {
    case LEX_NODE:
      expr = GET_NODE(AST_SELECTOR);
      RETURN_IF_NULL(expr);
      expr->data.d_selector.val = CUR_TOKEN_LITERAL();
      expr->data.d_selector.len = CUR_TOKEN_LEN();
      CONSUME_TOKEN();
      break;
    case LEX_FILTER_START:
      expr = parse_filter(PARSER_ARGS);
      CONSUME_TOKEN();
      break;
    case LEX_EXPR_START:
      expr = parse_expression(PARSER_ARGS);
      if (expr == NULL) {
        return NULL;
      }
      CONSUME_TOKEN();
      break;
    case LEX_DEEP_SCAN:
      expr = GET_NODE(AST_RECURSE);
      RETURN_IF_NULL(expr);
      CONSUME_TOKEN();
      break;
    case LEX_WILD_CARD:
      expr = GET_NODE(AST_WILD_CARD);
      RETURN_IF_NULL(expr);
      CONSUME_TOKEN();
      break;
    case LEX_ROOT:
      /* Expressions like $.$ and $.node$ are not allowed. Bracket notation $['$'] and $['node$'] should be used
       * instead. */
      throw_jsonpath_exception(
          "Unexpected root `$` in node name, use bracket notation for node names with special characters");
      return NULL;
    default:
      throw_jsonpath_exception("Expecting child node, filter, expression, or recursive node");
      return NULL;
  }

  if (expr != NULL && HAS_TOKEN() && !is_logical_operator(CUR_TOKEN()) && CUR_TOKEN() != LEX_PAREN_CLOSE) {
    /* Make sure the next call to parse_operator() returns NULL only if the there's an error, not because the token type
     * isn't an operator. */
    expr->next = parse_operator(PARSER_ARGS);
    if (expr->next == NULL) {
      return NULL;
    }
  }

  return expr;
}

static struct ast_node* parse_expression(PARSER_PARAMS) {
  CONSUME_TOKEN(); /* LEX_EXPR_START */

  if (CUR_TOKEN() != LEX_PAREN_OPEN) {
    throw_jsonpath_exception("Missing opening paren `(`");
    return NULL;
  }

  struct ast_node* expr = GET_NODE(AST_EXPR);
  RETURN_IF_NULL(expr);
  expr->data.d_expression.head = parse_or(PARSER_ARGS);

  if (!validate_expression_head(expr->data.d_expression.head)) {
    return NULL;
  }

  return expr;
}

static struct ast_node* parse_filter(PARSER_PARAMS) {
  CONSUME_TOKEN();

  if (!HAS_TOKEN()) {
    throw_jsonpath_exception("Missing filter end `]`");
    return NULL;
  }

  struct ast_node* expr = NULL;

  switch (CUR_TOKEN()) {
    case LEX_CHILD_SEP:
    case LEX_LITERAL_NUMERIC:
    case LEX_LITERAL:
    case LEX_SLICE:
      expr = GET_NODE(AST_INDEX_LIST);
      RETURN_IF_NULL(expr);
      if (!parse_filter_list(PARSER_ARGS, expr)) {
        return NULL;
      }
      break;
    case LEX_WILD_CARD:
      expr = GET_NODE(AST_WILD_CARD);
      RETURN_IF_NULL(expr);
      break;
    case LEX_EXPR_END:
      throw_jsonpath_exception("Filter must not be empty");
      return NULL;
    default:
      throw_jsonpath_exception("Unexpected filter element");
      return NULL;
  }

  CONSUME_TOKEN();

  if (!HAS_TOKEN() || CUR_TOKEN() != LEX_EXPR_END) {
    throw_jsonpath_exception("Missing filter end `]`");
    return NULL;
  }

  return expr;
}

static struct ast_node* parse_or(PARSER_PARAMS) {
  struct ast_node* expr = parse_and(PARSER_ARGS);

  while (HAS_TOKEN() && CUR_TOKEN() == LEX_OR) {
    CONSUME_TOKEN();

    struct ast_node* right = parse_and(PARSER_ARGS);

    expr = GET_BINARY_NODE(AST_OR, expr, right);
    RETURN_IF_NULL(expr);
  }

  return expr;
}

static struct ast_node* parse_and(PARSER_PARAMS) {
  struct ast_node* expr = parse_equality(PARSER_ARGS);

  while (HAS_TOKEN() && CUR_TOKEN() == LEX_AND) {
    CONSUME_TOKEN();

    struct ast_node* right = parse_equality(PARSER_ARGS);

    expr = GET_BINARY_NODE(AST_AND, expr, right);
    RETURN_IF_NULL(expr);
  }

  return expr;
}

static struct ast_node* parse_equality(PARSER_PARAMS) {
  struct ast_node* expr = parse_comparison(PARSER_ARGS);

  while (HAS_TOKEN()) {
    enum ast_type type;

    if (CUR_TOKEN() == LEX_EQ) {
      type = AST_EQ;
    } else if (CUR_TOKEN() == LEX_NEQ) {
      type = AST_NE;
    } else {
      break;
    }

    CONSUME_TOKEN();

    struct ast_node* right = parse_comparison(PARSER_ARGS);

    if (right == NULL) {
      return NULL;
    }

    expr = GET_BINARY_NODE(type, expr, right);
    RETURN_IF_NULL(expr);
  }

  return expr;
}

static struct ast_node* parse_comparison(PARSER_PARAMS) {
  struct ast_node* expr = parse_unary(PARSER_ARGS);

  while (HAS_TOKEN()) {
    enum ast_type type;

    if (CUR_TOKEN() == LEX_GT) {
      type = AST_GT;
    } else if (CUR_TOKEN() == LEX_GTE) {
      type = AST_GTE;
    } else if (CUR_TOKEN() == LEX_LT) {
      type = AST_LT;
    } else if (CUR_TOKEN() == LEX_LTE) {
      type = AST_LTE;
    } else if (CUR_TOKEN() == LEX_RGXP) {
      type = AST_RGXP;
    } else {
      break;
    }

    CONSUME_TOKEN();

    struct ast_node* right = parse_unary(PARSER_ARGS);

    if (right == NULL) {
      return NULL;
    }

    expr = GET_BINARY_NODE(type, expr, right);
    RETURN_IF_NULL(expr);
  }

  return expr;
}

static struct ast_node* parse_unary(PARSER_PARAMS) {
  if (CUR_TOKEN() == LEX_NEGATION) {
    CONSUME_TOKEN();
    struct ast_node* expr = GET_NODE(AST_NEGATION);
    RETURN_IF_NULL(expr);
    expr->data.d_unary.right = parse_unary(PARSER_ARGS);
    return expr;
  }

  return parse_primary(PARSER_ARGS);
}

static struct ast_node* parse_primary(PARSER_PARAMS) {
  if (CUR_TOKEN() == LEX_LITERAL) {
    struct ast_node* ret = GET_NODE(AST_LITERAL);
    RETURN_IF_NULL(ret);

    ret->data.d_literal.str = zend_string_init(CUR_TOKEN_LITERAL(), CUR_TOKEN_LEN(), 0);

    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_NUMERIC) {
    struct ast_node* ret = GET_NODE(AST_DOUBLE);
    RETURN_IF_NULL(ret);
    if (!make_numeric_node(ret, CUR_TOKEN_LITERAL(), CUR_TOKEN_LEN())) {
      throw_jsonpath_exception("Unable to parse numeric");
      return NULL;
    }
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_BOOL) {
    struct ast_node* ret = GET_NODE(AST_BOOL);
    RETURN_IF_NULL(ret);

    if (strncasecmp("true", CUR_TOKEN_LITERAL(), 4) == 0) {
      ret->data.d_literal.value_bool = true;
    } else if (strncasecmp("false", CUR_TOKEN_LITERAL(), 5) == 0) {
      ret->data.d_literal.value_bool = false;
    } else {
      throw_jsonpath_exception("Expected `true` or `false` for boolean token");
      return NULL;
    }
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_NULL) {
    struct ast_node* ret = GET_NODE(AST_NULL);
    RETURN_IF_NULL(ret);
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_PAREN_OPEN) {
    CONSUME_TOKEN();
    struct ast_node* expr = parse_or(PARSER_ARGS);

    /* Abort if parsing the expression resulted in an exception */
    if (expr == NULL) {
      return NULL;
    }

    if (HAS_TOKEN() && CUR_TOKEN() == LEX_PAREN_CLOSE) {
      CONSUME_TOKEN();
      return expr;
    } else {
      throw_jsonpath_exception("Missing closing paren `)`");
      return NULL;
    }
  }

  /* JSONPaths inside filter expressions may not contain operators or parens because otherwise the boundary between
   * parent/child JSONPaths would be ambiguous. */
  if (CUR_TOKEN() == LEX_ROOT) {
    int start = CUR_POS();
    int stop = 0;

    /* Find the boundary of the JSONPath */
    while (HAS_TOKEN()) {
      if (is_logical_operator(CUR_TOKEN()) || CUR_TOKEN() == LEX_PAREN_CLOSE) {
        stop = CUR_POS();
        break;
      }
      CONSUME_TOKEN();
    }

    /* Run parse_jsonpath on a subset of the lex stream, until the boundary of the sub-JSONPath */
    return parse_jsonpath(lex_tok, &start, stop, pool);
  }

  if (CUR_TOKEN() == LEX_CUR_NODE) {
    struct ast_node* expr = GET_NODE(AST_CUR_NODE);
    RETURN_IF_NULL(expr);
    CONSUME_TOKEN();
    if (HAS_TOKEN() && !is_logical_operator(CUR_TOKEN()) && CUR_TOKEN() != LEX_PAREN_CLOSE) {
      expr->next = parse_operator(PARSER_ARGS);
      if (expr->next == NULL) {
        return NULL;
      }
    }
    return expr;
  }

  return parse_operator(PARSER_ARGS);
}

bool is_binary(enum ast_type type) {
  switch (type) {
    case AST_AND:
    case AST_EQ:
    case AST_GT:
    case AST_GTE:
    case AST_LT:
    case AST_LTE:
    case AST_NE:
    case AST_OR:
    case AST_RGXP:
      return true;
    default:
      return false;
  }
}

static bool validate_expression_head(struct ast_node* tok) {
  if (tok == NULL) {
    return false;
  }

  if (is_binary(tok->type)) {
    return true;
  }

  if (is_unary(tok->type)) {
    return true;
  }

  if (tok->type == AST_SELECTOR || tok->type == AST_CUR_NODE) {
    return true;
  }

  throw_jsonpath_exception("Invalid expression.");

  return false;
}

static bool make_numeric_node(struct ast_node* tok, char* str, int str_len) {
  zend_long lval;
  double dval;
  int oflow_info;
  zend_uchar res;

#if PHP_MAJOR_VERSION >= 8
  res = _is_numeric_string_ex(str, str_len, &lval, &dval, false, &oflow_info, NULL);
#else
  res = _is_numeric_string_ex(str, str_len, &lval, &dval, false, &oflow_info);
#endif

  /* did the conversion overflow? */
  if (oflow_info != 0) {
    return false;
  }

  switch (res) {
    case IS_LONG:
      tok->type = AST_LONG;
      tok->data.d_long.value = lval;
      return true;
    case IS_DOUBLE:
      tok->type = AST_DOUBLE;
      tok->data.d_double.value = dval;
      return true;
    default:
      return false;
  }
}

static bool numeric_to_long(char* str, int str_len, long* dest) {
  zend_long lval;
  double dval;
  int oflow_info;
  zend_uchar res;

#if PHP_MAJOR_VERSION >= 8
  res = _is_numeric_string_ex(str, str_len, &lval, &dval, false, &oflow_info, NULL);
#else
  res = _is_numeric_string_ex(str, str_len, &lval, &dval, false, &oflow_info);
#endif

  /* did the conversion overflow? */
  if (oflow_info != 0) {
    return false;
  }

  if (res != IS_LONG) {
    return false;
  }

  *dest = lval;

  return true;
}

static bool is_logical_operator(lex_token type) {
  switch (type) {
    case LEX_AND:
    case LEX_EQ:
    case LEX_GT:
    case LEX_GTE:
    case LEX_LT:
    case LEX_LTE:
    case LEX_NEQ:
    case LEX_OR:
    case LEX_RGXP:
      return true;
    default:
      return false;
  }
}

void free_php_objects(struct node_pool* pool) {
  int i;

  for (i = 0; i < pool->cur_index; i++) {
    if (pool->nodes[i].type == AST_LITERAL) {
      zend_string_release(pool->nodes[i].data.d_literal.str);
    } else if (pool->nodes[i].type == AST_NODE_LIST || pool->nodes[i].type == AST_INDEX_LIST ||
               pool->nodes[i].type == AST_INDEX_SLICE) {
      zend_array_destroy(pool->nodes[i].data.d_list.ht);
    }
  }
}

#ifdef JSONPATH_DEBUG
void print_ast(struct ast_node* head, const char* m, int level) {
  int i;

  if (level == 0) {
    printf("--------------------------------------\n");
    printf("%s\n\n", m);
    print_ast(head, m, level + 1);
    return;
  }

  while (head != NULL) {
    for (i = 0; i < level; i++) {
      printf("\t");
    }
    printf("➔ %s", AST_STR[head->type]);
    switch (head->type) {
      case AST_AND:
      case AST_EQ:
      case AST_GT:
      case AST_GTE:
      case AST_LT:
      case AST_LTE:
      case AST_NE:
      case AST_OR:
      case AST_RGXP:
        printf("\n");
        print_ast(head->data.d_binary.left, m, level + 1);
        print_ast(head->data.d_binary.right, m, level + 1);
        break;
      case AST_BOOL:
        printf(" [val=%d]\n", head->data.d_literal.value_bool);
        break;
      case AST_EXPR:
        printf("\n");
        print_ast(head->data.d_expression.head, m, level + 1);
        break;
      case AST_LONG:
        printf(" [val=%ld]\n", head->data.d_long.value);
        break;
      case AST_DOUBLE:
        printf(" [val=%f]\n", head->data.d_double.value);
        break;
      case AST_NULL:
        printf(" [val=null]\n");
        break;
      case AST_SELECTOR:
        while (head->type == AST_SELECTOR) {
          printf(" [val=%.*s]", head->data.d_selector.len, head->data.d_selector.val);
          if (head->next != NULL && head->next->type == AST_SELECTOR) {
            head = head->next;
          } else {
            break;
          }
        }
        printf("\n");
        break;
      case AST_LITERAL:
        printf(" [val=%.*s]\n", (int)head->data.d_literal.str->len, head->data.d_literal.str->val);
        break;
      case AST_INDEX_SLICE:
        printf(" [start=%d end=%d step=%d]\n", (int)Z_LVAL_P(zend_hash_index_find(head->data.d_list.ht, 0)),
               (int)Z_LVAL_P(zend_hash_index_find(head->data.d_list.ht, 1)),
               (int)Z_LVAL_P(zend_hash_index_find(head->data.d_list.ht, 2)));
        break;
      case AST_NEGATION:
        printf("\n");
        print_ast(head->data.d_unary.right, m, level + 1);
        break;
      default:
        printf("\n");
    }
    head = head->next;
  }
}
#endif