#include "parser.h"

#include <limits.h>
#include <stdio.h>

#include <ext/spl/spl_exceptions.h>

#include "safe_string.h"
#include "zend_exceptions.h"

#define CONSUME_TOKEN() (*lex_idx)++
#define CUR_POS() *lex_idx
#define CUR_TOKEN_LITERAL() lex_tok_values[*lex_idx]
#define CUR_TOKEN() lex_token[*lex_idx]
#define HAS_TOKEN() *lex_idx < lex_tok_count
#define PARSER_ARGS lex_token, lex_tok_values, lex_idx, lex_tok_count
#define PARSER_PARAMS \
  lex_token lex_token[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int *lex_idx, int lex_tok_count

static struct ast_node* ast_alloc_binary(enum ast_type type, struct ast_node* left, struct ast_node* right);
static struct ast_node* ast_alloc_node(struct ast_node* prev, enum ast_type type);

static struct ast_node* parse_expression(PARSER_PARAMS);
static struct ast_node* parse_or(PARSER_PARAMS);
static struct ast_node* parse_and(PARSER_PARAMS);
static struct ast_node* parse_equality(PARSER_PARAMS);
static struct ast_node* parse_comparison(PARSER_PARAMS);
static struct ast_node* parse_primary(PARSER_PARAMS);
static struct ast_node* parse_unary(PARSER_PARAMS);

static bool parse_filter_list(lex_token lex_token[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* start,
                              int lex_tok_count, struct ast_node* tok);
static bool validate_root_next(struct ast_node* head);

static bool numeric_to_long(char* str, int str_len, long* dest);
static bool is_operator(lex_token type);
static bool make_numeric_node(struct ast_node* tok, char* str, int str_len);

const char* AST_STR[] = {"AST_AND",  "AST_BOOL",       "AST_DOUBLE",      "AST_EQ",          "AST_EXPR",
                         "AST_GT",   "AST_GTE",        "AST_INDEX_LIST",  "AST_INDEX_SLICE", "AST_LITERAL",
                         "AST_LONG", "AST_LT",         "AST_LTE",         "AST_NE",          "AST_NEGATION",
                         "AST_OR",   "AST_PAREN_LEFT", "AST_PAREN_RIGHT", "AST_RECURSE",     "AST_RGXP",
                         "AST_ROOT", "AST_SELECTOR",   "AST_WILD_CARD"};

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

bool build_parse_tree(lex_token lex_token[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                      int lex_tok_count, struct ast_node* head) {
  struct ast_node* cur = head;

  for (; *lex_idx < lex_tok_count; (*lex_idx)++) {
    switch (CUR_TOKEN()) {
      case LEX_WILD_CARD:
        cur = ast_alloc_node(cur, AST_WILD_CARD);
        break;
      case LEX_ROOT:
        cur = ast_alloc_node(cur, AST_ROOT);
        break;
      case LEX_DEEP_SCAN:
        cur = ast_alloc_node(cur, AST_RECURSE);
        break;
      case LEX_CUR_NODE:
        // noop
        break;
      case LEX_NODE:
        // fall-through
        cur = ast_alloc_node(cur, AST_SELECTOR);
        strcpy(cur->data.d_selector.value, CUR_TOKEN_LITERAL());
        break;
      case LEX_FILTER_START:

        if (*lex_idx == lex_tok_count - 1) { /* last token */
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing filter end ]");
          return false;
        }

        CONSUME_TOKEN();

        switch (CUR_TOKEN()) {
          case LEX_LITERAL_NUMERIC:
            /* fall-through */
          case LEX_LITERAL:
            /* fall-through */
          case LEX_SLICE:
            /* fall-through */
          case LEX_CHILD_SEP:
            cur = ast_alloc_node(cur, AST_INDEX_LIST);
            if (!parse_filter_list(lex_token, lex_tok_values, lex_idx, lex_tok_count, cur)) {
              return false;
            }
            break;
          case LEX_WILD_CARD:
            cur = ast_alloc_node(cur, AST_WILD_CARD);
            break;
          case LEX_EXPR_END:
            zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Filter must not be empty");
            return false;
          default:
            /* noop */
            break;
        }

        if (*lex_idx == lex_tok_count - 1 || lex_token[(*lex_idx) + 1] != LEX_EXPR_END) { /* last token */
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing filter end ]");
          return false;
        }

        CONSUME_TOKEN();

        break;
      case LEX_EXPR_START:
        cur->next = parse_expression(PARSER_ARGS);
        cur = cur->next;

        if (cur->data.d_expression.head == NULL) {
          return false;
        }

#ifdef JSONPATH_DEBUG
        print_ast(cur->data.d_expression.head, "Parser - Expression before infix-postfix conversion", 0);
#endif
        break;
      default:
        /* this token is probably in an invalid position. the problem should */
        /* be caught later by validate_parse_tree. */
        break;
    }
  }

  return true;
}

static bool parse_filter_list(lex_token lex_token[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                              int lex_tok_count, struct ast_node* tok) {
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
                             "Multiple filter list separators found [,:], only one type is allowed.", 0);
        return false;
      }
      tok->type = sep_found = AST_INDEX_LIST;
    } else if (CUR_TOKEN() == LEX_SLICE) {
      if (sep_found == AST_INDEX_LIST) {
        zend_throw_exception(spl_ce_RuntimeException,
                             "Multiple filter list separators found [,:], only one type is allowed.", 0);
        return false;
      }

      tok->type = sep_found = AST_INDEX_SLICE;

      slice_count++;
      // [:a] => [0:a]
      // [a::] => [a:0:]
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

      if (!numeric_to_long(CUR_TOKEN_LITERAL(), strlen(CUR_TOKEN_LITERAL()), &idx)) {
        zend_throw_exception(spl_ce_RuntimeException, "Unable to parse filter index value.", 0);
        return false;
      }

      tok->data.d_list.indexes[tok->data.d_list.count] = idx;
      tok->data.d_list.count++;
    } else {
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unexpected token in filter: %s", LEX_STR[CUR_TOKEN()]);
      return false;
    }
  }
  return true;
}

static struct ast_node* parse_expression(PARSER_PARAMS) {
  CONSUME_TOKEN(); /* LEX_EXPR_START */

  struct ast_node* expr = ast_alloc_node(NULL, AST_EXPR);
  expr->data.d_expression.head = parse_or(lex_token, lex_tok_values, lex_idx, lex_tok_count);

  return expr;
}

static struct ast_node* parse_or(PARSER_PARAMS) {
  struct ast_node* expr = parse_and(PARSER_ARGS);

  while (CUR_TOKEN() == LEX_OR) {
    CONSUME_TOKEN();

    struct ast_node* right = parse_and(PARSER_ARGS);

    expr = ast_alloc_binary(AST_OR, expr, right);
  }

  return expr;
}

static struct ast_node* parse_and(PARSER_PARAMS) {
  struct ast_node* expr = parse_equality(PARSER_ARGS);

  while (CUR_TOKEN() == LEX_AND) {
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
  if (CUR_TOKEN() == LEX_CUR_NODE) {
    CONSUME_TOKEN();
  }

  if (CUR_TOKEN() == LEX_LITERAL) {
    struct ast_node* ret = ast_alloc_node(NULL, AST_LITERAL);

    if (jp_str_cpy(ret->data.d_literal.value, PARSE_BUF_LEN, CUR_TOKEN_LITERAL(), strlen(CUR_TOKEN_LITERAL())) > 0) {
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Buffer size exceeded");
      return NULL;
    }
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_LITERAL_NUMERIC) {
    struct ast_node* ret = ast_alloc_node(NULL, AST_DOUBLE);
    if (!make_numeric_node(ret, CUR_TOKEN_LITERAL(), strlen(CUR_TOKEN_LITERAL()))) {
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unable to parse numeric.");
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
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Expected `true` or `false` for boolean token.");
      return NULL;
    }
    CONSUME_TOKEN();
    return ret;
  }

  if (CUR_TOKEN() == LEX_NODE) {
    struct ast_node* ret = NULL;
    struct ast_node* tail = NULL;

    /* handle @.node */
    while (CUR_TOKEN() == LEX_NODE) {
      if (ret == NULL) {
        ret = ast_alloc_node(NULL, AST_SELECTOR);
        tail = ret;
      } else {
        tail = ast_alloc_node(tail, AST_SELECTOR);
      }
      strcpy(tail->data.d_selector.value, CUR_TOKEN_LITERAL());
      CONSUME_TOKEN();
    }

    /* handle @.node[?(...)] */
    if (CUR_TOKEN() == LEX_EXPR_START) {
      tail->next = parse_expression(PARSER_ARGS);
      tail = tail->next;

      CONSUME_TOKEN();

      if (tail->data.d_expression.head == NULL) {
        return NULL;
      }
    }

    return ret;
  }

  if (CUR_TOKEN() == LEX_PAREN_OPEN) {
    CONSUME_TOKEN();
    struct ast_node* expr = parse_or(PARSER_ARGS);
    if (CUR_TOKEN() == LEX_PAREN_CLOSE) {
      CONSUME_TOKEN();
      return expr;
    } else {
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing closing paren )");
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
      if (is_operator(CUR_TOKEN()) || CUR_TOKEN() == LEX_PAREN_CLOSE) {
        stop = CUR_POS();
        break;
      }
      CONSUME_TOKEN();
    }

    struct ast_node head = {0};
    struct ast_node* ptr = &head;

    /* Run build_parse_tree on a subset of the lex stream, until the */
    /* boundary of the sub-JSONPath */
    if (!build_parse_tree(lex_token, lex_tok_values, &start, stop, ptr)) {
      return NULL;
    }

    return head.next;
  }

  zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Filter expressions may not be empty.");

  return NULL;
}

static bool validate_root_next(struct ast_node* head) {
  switch (head->type) {
    case AST_EXPR:
    case AST_INDEX_LIST:
    case AST_INDEX_SLICE:
    case AST_RECURSE:
    case AST_SELECTOR:
    case AST_WILD_CARD:
      return true;
    default:
      return false;
  }
}

bool validate_parse_tree(struct ast_node* head) {
  struct ast_node* cur = head;

  while (cur != NULL) {
    switch (cur->type) {
      case AST_ROOT:
        if (cur->next == NULL) {
          return true;
        }
        if (!validate_root_next(cur->next)) {
          zend_throw_exception(spl_ce_RuntimeException,
                               "$ must be followed by a child selector, filter or recurse element.", 0);
          return false;
        }
        break;
      case AST_EXPR:
        if (cur->data.d_expression.head == NULL) {
          zend_throw_exception(spl_ce_RuntimeException, "Filter expressions may not be empty.", 0);
          return false;
        }
        break;
      case AST_RECURSE:
        if (cur->next == NULL || (cur->next->type == AST_SELECTOR && cur->next->data.d_selector.value[0] == '\0')) {
          zend_throw_exception(
              spl_ce_RuntimeException,
              "Recursive descent operator (..) must be followed by a child selector, filter or wildcard.", 0);
          return false;
        }
        break;
      default:
        break;
    }
    cur = cur->next;
  }

  return true;
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

static bool is_operator(lex_token type) {
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

bool sanity_check(lex_token lex_token[], int lex_tok_count) {
  if (lex_tok_count == 0) {
    zend_throw_exception(spl_ce_RuntimeException, "The JSONpath contains no valid elements", 0);
    return false;
  }

  if (lex_token[0] != LEX_ROOT) {
    zend_throw_exception(spl_ce_RuntimeException, "JSONpath must start with a root $", 0);
    return false;
  }

  return true;
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
  if (level == 0) {
    printf("--------------------------------------\n");
    printf("%s\n\n", m);
    print_ast(head, m, level + 1);
    return;
  }

  while (head != NULL) {
    for (int i = 0; i < level; i++) {
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
      case AST_SELECTOR:
        while (head->type == AST_SELECTOR) {
          printf(" [val=%s]", head->data.d_selector.value);
          if (head->next != NULL && head->next->type == AST_SELECTOR) {
            head = head->next;
          } else {
            break;
          }
        }
        printf("\n");
        break;
      case AST_LITERAL:
        printf(" [val=%s]\n", head->data.d_literal.value);
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