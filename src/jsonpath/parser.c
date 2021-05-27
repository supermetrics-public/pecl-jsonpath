#include "parser.h"

#include <limits.h>
#include <stdio.h>

#include <ext/spl/spl_exceptions.h>

#include "zend_exceptions.h"

#define CONSUME_TOKEN() (*lex_idx)++
#define CUR_POS() *lex_idx
#define CUR_TOKEN_LITERAL() lex_tok[*lex_idx].val
#define CUR_TOKEN_LEN() lex_tok[*lex_idx].len
#define CUR_TOKEN() lex_tok[*lex_idx].type
#define HAS_TOKEN() (*lex_idx < lex_tok_count)
#define PARSER_ARGS lex_tok, lex_idx, lex_tok_count
#define PARSER_PARAMS struct jpath_token lex_tok[PARSE_BUF_LEN], int *lex_idx, int lex_tok_count

static struct ast_node* ast_alloc_binary(enum ast_type type, struct ast_node* left, struct ast_node* right);
static struct ast_node* ast_alloc_node(struct ast_node* prev, enum ast_type type);

static struct ast_node* parse_and(PARSER_PARAMS);
static struct ast_node* parse_childpath(PARSER_PARAMS);
static struct ast_node* parse_comparison(PARSER_PARAMS);
static struct ast_node* parse_equality(PARSER_PARAMS);
static struct ast_node* parse_expression(PARSER_PARAMS);
static struct ast_node* parse_filter(PARSER_PARAMS);
static struct ast_node* parse_operator(PARSER_PARAMS);
static struct ast_node* parse_or(PARSER_PARAMS);
static struct ast_node* parse_primary(PARSER_PARAMS);
static struct ast_node* parse_unary(PARSER_PARAMS);

static bool parse_filter_list(PARSER_PARAMS, struct ast_node* tok);

static bool numeric_to_long(char* str, int str_len, long* dest);
static bool is_logical_operator(lex_token type);
static bool make_numeric_node(struct ast_node* tok, char* str, int str_len);
bool validate_expression_head(struct ast_node* tok);

const char* AST_STR[] = {"AST_AND",      "AST_BOOL", "AST_CUR_NODE", "AST_DOUBLE",     "AST_EQ",
                         "AST_EXPR",     "AST_GT",   "AST_GTE",      "AST_INDEX_LIST", "AST_INDEX_SLICE",
                         "AST_LITERAL",  "AST_LONG", "AST_LT",       "AST_LTE",        "AST_NE",
                         "AST_NEGATION", "AST_NULL", "AST_OR",       "AST_PAREN_LEFT", "AST_PAREN_RIGHT",
                         "AST_RECURSE",  "AST_RGXP", "AST_ROOT",     "AST_SELECTOR",   "AST_WILD_CARD"};

static struct ast_node* ast_alloc_binary(enum ast_type type, struct ast_node* left, struct ast_node* right) {
  struct ast_node* node = ast_alloc_node(NULL, type);

  node->data.d_binary.left = left;
  node->data.d_binary.right = right;

  return node;
}

static struct ast_node* ast_alloc_node(struct ast_node* prev, enum ast_type type) {
  struct ast_node* node = emalloc(sizeof(struct ast_node));
  memset(node, 0, sizeof(struct ast_node));

  node->type = type;
  if (prev != NULL) {
    prev->next = node;
  }

  return node;
}

static bool parse_filter_list(PARSER_PARAMS, struct ast_node* tok) {
  int slice_count = 0;

  /* assume filter type is an index list by default. this resolves type */
  /* ambiguity of a filter containing no separators. */
  /* example: treat level4[0] as an index filter, not a slice. */
  tok->type = AST_INDEX_LIST;
  /* used to determine if different separator types are present, default value is arbitrary */
  enum ast_type sep_found = AST_AND;

  for (; *lex_idx < lex_tok_count; (*lex_idx)++) {
    if (CUR_TOKEN() == LEX_EXPR_END) {
      /* lex_idx must point to LEX_EXPR_END after function returns */
      (*lex_idx)--;
      break;
    } else if (CUR_TOKEN() == LEX_CHILD_SEP) {
      if (sep_found == AST_INDEX_SLICE) {
        zend_throw_exception(spl_ce_RuntimeException,
                             "Multiple filter list separators `,` and `:` found, only one type is allowed", 0);
        return false;
      }
      tok->type = sep_found = AST_INDEX_LIST;
    } else if (CUR_TOKEN() == LEX_SLICE) {
      if (sep_found == AST_INDEX_LIST) {
        zend_throw_exception(spl_ce_RuntimeException,
                             "Multiple filter list separators `,` and `:` found, only one type is allowed", 0);
        return false;
      }

      tok->type = sep_found = AST_INDEX_SLICE;

      slice_count++;
      /* [:a] => [0:a] */
      /* [a::] => [a:0:] */
      if (slice_count > tok->data.d_list.count) {
        if (slice_count == 1) {
          tok->data.d_list.indexes[tok->data.d_list.count] = INT_MAX;
        } else if (slice_count == 2) {
          tok->data.d_list.indexes[tok->data.d_list.count] = INT_MAX;
        }
        tok->data.d_list.count++;
      }
    } else if (CUR_TOKEN() == LEX_LITERAL_NUMERIC) {
      long idx = 0;

      if (!numeric_to_long(CUR_TOKEN_LITERAL(), CUR_TOKEN_LEN(), &idx)) {
        zend_throw_exception(spl_ce_RuntimeException, "Unable to parse filter index value", 0);
        return false;
      }

      tok->data.d_list.indexes[tok->data.d_list.count] = idx;
      tok->data.d_list.count++;
    } else {
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unexpected token `%s` in filter", LEX_STR[CUR_TOKEN()]);
      return false;
    }
  }
  return true;
}

struct ast_node* parse_jsonpath(PARSER_PARAMS) {
  if (!HAS_TOKEN() || CUR_TOKEN() != LEX_ROOT) {
    zend_throw_exception_ex(spl_ce_RuntimeException, 0, "JSONPath must start with a root operator `$`");
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

  struct ast_node* ret = ast_alloc_node(NULL, AST_ROOT);
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
      expr = ast_alloc_node(NULL, AST_SELECTOR);
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
      expr = ast_alloc_node(NULL, AST_RECURSE);
      CONSUME_TOKEN();
      /* todo : why is LEX_NOT_FOUND generated in tests/comparison_recursive_descent/002.php? */
      if (!HAS_TOKEN() || (CUR_TOKEN() == LEX_NODE && CUR_TOKEN_LEN() == 0) || CUR_TOKEN() == LEX_NOT_FOUND) {
        zend_throw_exception(spl_ce_RuntimeException,
                             "Recursive descent operator `..` must be followed by a child selector, filter or wildcard",
                             0);
        return NULL;
      }
      break;
    case LEX_WILD_CARD:
      expr = ast_alloc_node(NULL, AST_WILD_CARD);
      CONSUME_TOKEN();
      break;
    case LEX_ROOT:
      /* Expressions like $.$ and $.node$ are not allowed. Bracket notation $['$'] and $['node$'] should be used
       * instead. */
      zend_throw_exception(
          spl_ce_RuntimeException,
          "Unexpected root `$` in node name, use bracket notation for node names with special characters", 0);
      return NULL;
    default:
      zend_throw_exception_ex(spl_ce_RuntimeException, 0,
                              "Expecting child node, filter, expression, or recursive node");
      return NULL;
  }

  if (expr != NULL && HAS_TOKEN() && !is_logical_operator(CUR_TOKEN()) && CUR_TOKEN() != LEX_PAREN_CLOSE) {
    /* Make sure the next call to parse_operator() returns NULL only if the there's an error, not because the token type
     * isn't an operator. */
    expr->next = parse_operator(PARSER_ARGS);
    if (expr->next == NULL) {
      free_ast_nodes(expr);
      return NULL;
    }
  }

  return expr;
}

static struct ast_node* parse_expression(PARSER_PARAMS) {
  CONSUME_TOKEN(); /* LEX_EXPR_START */

  if (CUR_TOKEN() != LEX_PAREN_OPEN) {
    zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing opening paren `(`");
    return NULL;
  }

  struct ast_node* expr = ast_alloc_node(NULL, AST_EXPR);
  expr->data.d_expression.head = parse_or(PARSER_ARGS);

  if (!validate_expression_head(expr->data.d_expression.head)) {
    free_ast_nodes(expr);
    return NULL;
  }

  return expr;
}

static struct ast_node* parse_filter(PARSER_PARAMS) {
  CONSUME_TOKEN();

  if (!HAS_TOKEN()) {
    zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing filter end `]`");
    return NULL;
  }

  struct ast_node* expr = NULL;

  switch (CUR_TOKEN()) {
    case LEX_CHILD_SEP:
    case LEX_LITERAL_NUMERIC:
    case LEX_LITERAL:
    case LEX_SLICE:
      expr = ast_alloc_node(NULL, AST_INDEX_LIST);
      if (!parse_filter_list(PARSER_ARGS, expr)) {
        free_ast_nodes(expr);
        return NULL;
      }
      break;
    case LEX_WILD_CARD:
      expr = ast_alloc_node(NULL, AST_WILD_CARD);
      break;
    case LEX_EXPR_END:
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Filter must not be empty");
      return NULL;
    default:
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unexpected filter element");
      return NULL;
  }

  CONSUME_TOKEN();

  if (!HAS_TOKEN() || CUR_TOKEN() != LEX_EXPR_END) {
    free_ast_nodes(expr);
    zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing filter end `]`");
    return NULL;
  }

  return expr;
}

static struct ast_node* parse_or(PARSER_PARAMS) {
  struct ast_node* expr = parse_and(PARSER_ARGS);

  while (HAS_TOKEN() && CUR_TOKEN() == LEX_OR) {
    CONSUME_TOKEN();

    struct ast_node* right = parse_and(PARSER_ARGS);

    expr = ast_alloc_binary(AST_OR, expr, right);
  }

  return expr;
}

static struct ast_node* parse_and(PARSER_PARAMS) {
  struct ast_node* expr = parse_equality(PARSER_ARGS);

  while (HAS_TOKEN() && CUR_TOKEN() == LEX_AND) {
    CONSUME_TOKEN();

    struct ast_node* right = parse_equality(PARSER_ARGS);

    expr = ast_alloc_binary(AST_AND, expr, right);
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
      free_ast_nodes(expr);
      free_ast_nodes(right);
      return NULL;
    }

    expr = ast_alloc_binary(type, expr, right);
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
      free_ast_nodes(expr);
      free_ast_nodes(right);
      return NULL;
    }

    expr = ast_alloc_binary(type, expr, right);
  }

  return expr;
}

static struct ast_node* parse_unary(PARSER_PARAMS) {
  if (CUR_TOKEN() == LEX_NEGATION) {
    CONSUME_TOKEN();
    struct ast_node* expr = ast_alloc_node(NULL, AST_NEGATION);
    expr->data.d_unary.right = parse_unary(PARSER_ARGS);
    return expr;
  }

  return parse_primary(PARSER_ARGS);
}

static struct ast_node* parse_primary(PARSER_PARAMS) {
  if (CUR_TOKEN() == LEX_LITERAL) {
    struct ast_node* ret = ast_alloc_node(NULL, AST_LITERAL);

    ret->data.d_literal.val = CUR_TOKEN_LITERAL();
    ret->data.d_literal.len = CUR_TOKEN_LEN();

    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_NUMERIC) {
    struct ast_node* ret = ast_alloc_node(NULL, AST_DOUBLE);
    if (!make_numeric_node(ret, CUR_TOKEN_LITERAL(), CUR_TOKEN_LEN())) {
      free_ast_nodes(ret);
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unable to parse numeric");
      return NULL;
    }
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_BOOL) {
    struct ast_node* ret = ast_alloc_node(NULL, AST_BOOL);

    if (strncasecmp("true", CUR_TOKEN_LITERAL(), 4) == 0) {
      ret->data.d_literal.value_bool = true;
    } else if (strncasecmp("false", CUR_TOKEN_LITERAL(), 5) == 0) {
      ret->data.d_literal.value_bool = false;
    } else {
      free_ast_nodes(ret);
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Expected `true` or `false` for boolean token");
      return NULL;
    }
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_NULL) {
    struct ast_node* ret = ast_alloc_node(NULL, AST_NULL);
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
      free_ast_nodes(expr);
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing closing paren `)`");
      return NULL;
    }
  }

  /* JSONPaths inside filter expressions may not contain operators or */
  /* parens because otherwise the boundary between parent/child JSONPaths */
  /* would be ambiguous. */
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

    /* Run parse_jsonpath on a subset of the lex stream, until the */
    /* boundary of the sub-JSONPath */
    return parse_jsonpath(lex_tok, &start, stop);
  }

  if (CUR_TOKEN() == LEX_CUR_NODE) {
    struct ast_node* expr = ast_alloc_node(NULL, AST_CUR_NODE);
    CONSUME_TOKEN();
    if (HAS_TOKEN() && !is_logical_operator(CUR_TOKEN()) && CUR_TOKEN() != LEX_PAREN_CLOSE) {
      expr->next = parse_operator(PARSER_ARGS);
      if (expr->next == NULL) {
        free_ast_nodes(expr);
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

bool is_unary(enum ast_type type) {
  switch (type) {
    case AST_NEGATION:
      return true;
    default:
      return false;
  }
}

bool validate_expression_head(struct ast_node* tok) {
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

  zend_throw_exception(spl_ce_RuntimeException, "Invalid expression.", 0);

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
  }

  return false;
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

void free_ast_nodes(struct ast_node* head) {
  if (head == NULL) {
    return;
  }

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
      free_ast_nodes(head->data.d_binary.left);
      free_ast_nodes(head->data.d_binary.right);
      break;
    case AST_EXPR:
      free_ast_nodes(head->data.d_expression.head);
      break;
    case AST_NEGATION:
      free_ast_nodes(head->data.d_unary.right);
      break;
    default:
      /* noop */
      break;
  }

  free_ast_nodes(head->next);

  efree((void*)head);
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
        printf(" [val=%.*s]\n", head->data.d_literal.len, head->data.d_literal.val);
        break;
      case AST_INDEX_SLICE:
        printf(" [start=%d end=%d step=%d]\n", head->data.d_list.indexes[0], head->data.d_list.indexes[1],
               head->data.d_list.indexes[2]);
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