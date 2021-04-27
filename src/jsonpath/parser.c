#include "parser.h"

#include <limits.h>
#include <stdio.h>

#include <ext/spl/spl_exceptions.h>

#include "safe_string.h"
#include "stack.h"
#include "zend_exceptions.h"

bool convert_to_postfix(struct ast_node* expr_start);
bool is_unary(enum ast_type);
int get_operator_precedence(struct ast_node* tok);
int get_operator_precedence(struct ast_node* tok);
struct ast_node* ast_alloc_node(struct ast_node* prev, enum ast_type type);
void delete_expression_head_node(struct ast_node* expr);
bool parse_filter_list(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* start,
                       int lex_tok_count, struct ast_node* tok);
#ifdef JSONPATH_DEBUG
void print_ast(struct ast_node* head, const char* m, int level);
#endif
bool check_parens_balance(lex_token lex_tok[], int lex_tok_count);
bool validate_root_next(struct ast_node* head);
void group_operands_under_value(struct ast_node* head);
void insert_isset_nodes(struct ast_node* head);
bool is_isset_operand(struct ast_node* prev, struct ast_node* cur);
bool numeric_to_long(char* str, int str_len, long* dest);

const char* AST_STR[] = {"AST_AND",     "AST_BOOL",       "AST_DOUBLE",      "AST_EQ",          "AST_EXPR",
                         "AST_GT",      "AST_GTE",        "AST_INDEX_LIST",  "AST_INDEX_SLICE", "AST_ISSET",
                         "AST_LITERAL", "AST_LONG",       "AST_LT",          "AST_LTE",         "AST_NE",
                         "AST_OR",      "AST_PAREN_LEFT", "AST_PAREN_RIGHT", "AST_RECURSE",     "AST_RGXP",
                         "AST_ROOT",    "AST_SELECTOR",   "AST_WILD_CARD",   "AST_HEAD",        "AST_VALUE"};

struct ast_node* ast_alloc_node(struct ast_node* prev, enum ast_type type) {
  struct ast_node* next = emalloc(sizeof(struct ast_node));
  memset(next, 0, sizeof(struct ast_node));

  next->type = type;
  prev->next = next;

  return next;
}

void delete_expression_head_node(struct ast_node* expr) {
  struct ast_node* tmp = expr->data.d_expression.head;

  if (tmp == NULL) {
    return;
  }

  expr->data.d_expression.head = expr->data.d_expression.head->next;

  efree((void*)tmp);
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
      case AST_VALUE:
        printf("\n");
        print_ast(head->data.d_value.head, m, level + 1);
        break;
      case AST_SELECTOR:
        printf(" [val=%s]\n", head->data.d_selector.value);
        break;
      case AST_LITERAL:
        printf(" [val=%s]\n", head->data.d_literal.value);
        break;
      case AST_INDEX_SLICE:
        printf(" [start=%d end=%d step=%d]\n", head->data.d_list.indexes[0], head->data.d_list.indexes[1],
               head->data.d_list.indexes[2]);
        break;
      default:
        printf("\n");
    }
    head = head->next;
  }
}
#endif

bool is_isset_operand(struct ast_node* prev, struct ast_node* cur) {
  return (prev->type == AST_AND || prev->type == AST_OR || prev->type == AST_PAREN_LEFT) &&
         (cur->next == NULL || cur->next->type == AST_AND || cur->next->type == AST_OR ||
          cur->next->type == AST_PAREN_RIGHT);
}

/* Insert ISSET operators next to selectors where needed.                    */
/* Subexpressions consisting of only one selector operand are intended to    */
/* test the existence of a field. A selector followed by an ISSET operator   */
/* instructs to the interpreter to perform the field check. A selector is an */
/* ISSET operand when both left and right neighbor nodes are parens, OR      */
/* operators, or AND operators.                                              */
/* -Example-                                                                 */
/*  Before:                                                                  */
/*   HEAD->AST_PAREN_LEFT->AST_VALUE->AST_PAREN_RIGHT                        */
/*  After:                                                                   */
/*   HEAD->AST_PAREN_LEFT->AST_VALUE->AST_ISSET->AST_PAREN_RIGHT             */
void insert_isset_nodes(struct ast_node* head) {
  struct ast_node* prev = head;
  struct ast_node* cur = head->next;

  while (cur != NULL) {
    if (cur->type == AST_VALUE && cur->data.d_value.head->type == AST_SELECTOR) {
      if (is_isset_operand(prev, cur)) {
        struct ast_node* tmp = cur->next;
        cur = ast_alloc_node(cur, AST_ISSET);
        cur->next = tmp;
      }
    }
    prev = cur;
    cur = cur->next;
  }
}

/* Consolidate contiguous groups of operands under value nodes. This         */
/* simplifies evaluating expression values in the interpreter.               */
/* -Example-                                                                 */
/*  Before:                                                                  */
/*   HEAD->OPERAND1->OPERAND2->OPERAND3->OPERATOR->...                       */
/*  After:                                                                   */
/*   HEAD->VALUE->OPERATOR->...                                              */
/*          |                                                                */
/*           \                                                               */
/*         OPERAND1->OPERAND2->OPERAND3                                      */
void group_operands_under_value(struct ast_node* head) {
  struct ast_node* cur = head->next;

  while (cur->next != NULL) {
    if (get_token_type(cur->next->type) == TYPE_OPERAND) {
      /* insert value node before operand */
      struct ast_node* tmp = cur->next;
      cur = ast_alloc_node(cur, AST_VALUE);

      /* make the rest of the list a child of the value node */
      cur->data.d_value.head = tmp;

      /* keep a reference to the new value node */
      struct ast_node* value = cur;

      /* find the first paren or operator node */
      cur = cur->data.d_value.head;
      while (cur->next != NULL && get_token_type(cur->next->type) == TYPE_OPERAND) {
        cur = cur->next;
      }

      /* assign everything following the operands back to top level */
      value->next = cur->next;
      cur->next = NULL;
      cur = value->next;
    } else {
      cur = cur->next;
    }
  }
}

// See http://csis.pace.edu/~wolf/CS122/infix-postfix.htm
bool convert_to_postfix(struct ast_node* expr_start) {
  stack s = {0};
  stack_init(&s);
  struct ast_node* cur = expr_start->next;
  struct ast_node* tmp = NULL;
  struct ast_node* pfix = expr_start;

  while (cur != NULL) {
    switch (get_token_type(cur->type)) {
      case TYPE_OPERAND:
        pfix->next = cur;
        pfix = pfix->next;
        cur = cur->next;
        break;
      case TYPE_OPERATOR:
        // TODO check missing operand on RHS
        // if cur == NULL || (cur->next == NULL && !is_unary(cur->type)) {
        // }
        if (!s.size || ((struct ast_node*)stack_top(&s))->type == AST_PAREN_LEFT) {
          stack_push(&s, cur);
          cur = cur->next;
        } else {
          tmp = stack_top(&s);

          // TODO compare macro or assign to var?
          if (get_operator_precedence(cur) > get_operator_precedence(tmp)) {
            stack_push(&s, cur);
            cur = cur->next;
          } else if (get_operator_precedence(cur) < get_operator_precedence(tmp)) {
            pfix->next = tmp;
            pfix = pfix->next;
            stack_pop(&s);
          } else {
            pfix->next = tmp;
            pfix = pfix->next;

            stack_pop(&s);
            stack_push(&s, cur);
            cur = cur->next;
          }
        }
        break;
      case TYPE_PAREN:
        if (cur->type == AST_PAREN_LEFT) {
          stack_push(&s, cur);
          cur = cur->next;
        } else {
          while (s.size > 0) {
            tmp = stack_top(&s);
            stack_pop(&s);
            if (tmp->type == AST_PAREN_LEFT) {
              efree(tmp);
              break;
            }
            pfix->next = tmp;
            pfix = pfix->next;
          }
          /* free right paren */
          tmp = cur;
          cur = cur->next;
          efree(tmp);
        }
        break;
    }
  }

  /* remove remaining elements */
  while (s.size > 0) {
    tmp = stack_top(&s);
    pfix->next = tmp;
    pfix = pfix->next;
    stack_pop(&s);
  }

  pfix->next = cur;

  return true;
}

bool make_numeric_node(struct ast_node* tok, char* str, int str_len) {
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

bool build_parse_tree(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                      int lex_tok_count, struct ast_node* head) {
  struct ast_node* cur = head;

  for (; *lex_idx < lex_tok_count; (*lex_idx)++) {
    switch (lex_tok[*lex_idx]) {
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
        if (lex_tok[*lex_idx] == LEX_CUR_NODE) {
          cur->data.d_selector.child_scope = true;
        } else {
          cur->data.d_selector.child_scope = false;
        }
        strcpy(cur->data.d_selector.value, lex_tok_values[*lex_idx]);
        break;
      case LEX_FILTER_START:

        if (*lex_idx == lex_tok_count - 1) { /* last token */
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing filter end ]");
          return false;
        }

        (*lex_idx)++;

        switch (lex_tok[*lex_idx]) {
          case LEX_LITERAL_NUMERIC:
            /* fall-through */
          case LEX_LITERAL:
            /* fall-through */
          case LEX_SLICE:
            /* fall-through */
          case LEX_CHILD_SEP:
            cur = ast_alloc_node(cur, AST_INDEX_LIST);
            if (!parse_filter_list(lex_tok, lex_tok_values, lex_idx, lex_tok_count, cur)) {
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

        if (*lex_idx == lex_tok_count - 1 || lex_tok[(*lex_idx) + 1] != LEX_EXPR_END) { /* last token */
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Missing filter end ]");
          return false;
        }

        (*lex_idx)++;

        break;
      case LEX_PAREN_OPEN:
        cur = ast_alloc_node(cur, AST_PAREN_LEFT);
        break;
      case LEX_PAREN_CLOSE:
        cur = ast_alloc_node(cur, AST_PAREN_RIGHT);
        break;
      case LEX_LITERAL:
        cur = ast_alloc_node(cur, AST_LITERAL);

        if (jp_str_cpy(cur->data.d_literal.value, PARSE_BUF_LEN, lex_tok_values[*lex_idx],
                       strlen(lex_tok_values[*lex_idx])) > 0) {
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Buffer size exceeded");
          return false;
        }

        break;
      case LEX_LITERAL_NUMERIC:
        cur = ast_alloc_node(cur, AST_DOUBLE);
        if (!make_numeric_node(cur, lex_tok_values[*lex_idx], strlen(lex_tok_values[*lex_idx]))) {
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unable to parse numeric.");
          return false;
        }
        break;
      case LEX_LITERAL_BOOL:
        cur = ast_alloc_node(cur, AST_BOOL);

        if (strncasecmp("true", lex_tok_values[*lex_idx], 4) == 0) {
          cur->data.d_literal.value_bool = true;
        } else if (strncasecmp("false", lex_tok_values[*lex_idx], 5) == 0) {
          cur->data.d_literal.value_bool = false;
        } else {
          zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Expected `true` or `false` for boolean token.");
          return false;
        }
        break;
      case LEX_LT:
        cur = ast_alloc_node(cur, AST_LT);
        break;
      case LEX_LTE:
        cur = ast_alloc_node(cur, AST_LTE);
        break;
      case LEX_GT:
        cur = ast_alloc_node(cur, AST_GT);
        break;
      case LEX_GTE:
        cur = ast_alloc_node(cur, AST_GTE);
        break;
      case LEX_NEQ:
        cur = ast_alloc_node(cur, AST_NE);
        break;
      case LEX_EQ:
        cur = ast_alloc_node(cur, AST_EQ);
        break;
      case LEX_OR:
        cur = ast_alloc_node(cur, AST_OR);
        break;
      case LEX_AND:
        cur = ast_alloc_node(cur, AST_AND);
        break;
      case LEX_RGXP:
        cur = ast_alloc_node(cur, AST_RGXP);
        break;
      case LEX_EXPR_START:
        (*lex_idx)++;

        /** allocate dummy head.. **/
        /* I don't really love allocating a dummy head node, maybe we can do better */
        cur = ast_alloc_node(cur, AST_EXPR);
        cur->data.d_expression.head = emalloc(sizeof(struct ast_node));
        cur->data.d_expression.head->type = AST_HEAD;

        if (!build_parse_tree(lex_tok, lex_tok_values, lex_idx, lex_tok_count, cur->data.d_expression.head)) {
          return false;
        }

        group_operands_under_value(cur->data.d_expression.head);
        insert_isset_nodes(cur->data.d_expression.head);
#ifdef JSONPATH_DEBUG
        print_ast(cur->data.d_expression.head, "Parser - Expression before infix-postfix conversion", 0);
#endif
        convert_to_postfix(cur->data.d_expression.head);
        delete_expression_head_node(cur);
        break;
      case LEX_EXPR_END:
        /* return call initiated by LEX_EXPR_START */
        return true;
      default:
        /* this token is probably in an invalid position. the problem should */
        /* be caught later by validate_parse_tree. */
        break;
    }
  }

  return true;
}

bool validate_root_next(struct ast_node* head) {
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

bool numeric_to_long(char* str, int str_len, long* dest) {
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

bool parse_filter_list(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                       int lex_tok_count, struct ast_node* tok) {
  int slice_count = 0;

  /* assume filter type is an index list by default. this resolves type */
  /* ambiguity of a filter containing no separators. */
  /* example: treat level4[0] as an index filter, not a slice. */
  tok->type = AST_INDEX_LIST;
  /* used to determine if different separator types are present, default value is arbitrary */
  enum ast_type sep_found = AST_AND;

  for (; *lex_idx < lex_tok_count; (*lex_idx)++) {
    if (lex_tok[*lex_idx] == LEX_EXPR_END) {
      /* lex_idx must point to LEX_EXPR_END after function returns */
      (*lex_idx)--;
      break;
    } else if (lex_tok[*lex_idx] == LEX_CHILD_SEP) {
      if (sep_found == AST_INDEX_SLICE) {
        zend_throw_exception(spl_ce_RuntimeException,
                             "Multiple filter list separators found [,:], only one type is allowed.", 0);
        return false;
      }
      tok->type = sep_found = AST_INDEX_LIST;
    } else if (lex_tok[*lex_idx] == LEX_SLICE) {
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
    } else if (lex_tok[*lex_idx] == LEX_LITERAL_NUMERIC) {
      long idx = 0;

      if (!numeric_to_long(lex_tok_values[*lex_idx], strlen(lex_tok_values[*lex_idx]), &idx)) {
        zend_throw_exception(spl_ce_RuntimeException, "Unable to parse filter index value.", 0);
        return false;
      }

      tok->data.d_list.indexes[tok->data.d_list.count] = idx;
      tok->data.d_list.count++;
    } else {
      zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unexpected token in filter: %s", LEX_STR[lex_tok[*lex_idx]]);
      return false;
    }
  }
  return true;
}

operator_type get_token_type(enum ast_type type) {
  switch (type) {
    case AST_EQ:
    case AST_NE:
    case AST_LT:
    case AST_LTE:
    case AST_GT:
    case AST_GTE:
    case AST_OR:
    case AST_AND:
    case AST_ISSET:
    case AST_RGXP:
      return TYPE_OPERATOR;
    case AST_PAREN_LEFT:
    case AST_PAREN_RIGHT:
      return TYPE_PAREN;
    default:
      return TYPE_OPERAND;
  }
}

bool is_unary(enum ast_type type) { return type == AST_ISSET; }

// TODO: Distinguish between operator and token?
int get_operator_precedence(struct ast_node* tok) {
  switch (tok->type) {
    case AST_ISSET:
      return 10000;
    case AST_LT:
      return 1000;
    case AST_LTE:
      return 1000;
      break;
    case AST_GT:
      return 1000;
    case AST_GTE:
      return 1000;
    case AST_RGXP:
      return 1000;
    case AST_NE:
      return 900;
    case AST_EQ:
      return 900;
    case AST_AND:
      return 800;
    case AST_OR:
      return 700;
    case AST_PAREN_LEFT:
    case AST_PAREN_RIGHT:
    case AST_LITERAL:
    case AST_BOOL:
    default:
      assert(0);
      return -1;
  }
}

bool sanity_check(lex_token lex_tok[], int lex_tok_count) {
  if (lex_tok_count == 0) {
    zend_throw_exception(spl_ce_RuntimeException, "The JSONpath contains no valid elements", 0);
    return false;
  }

  if (lex_tok[0] != LEX_ROOT) {
    zend_throw_exception(spl_ce_RuntimeException, "JSONpath must start with a root $", 0);
    return false;
  }

  return check_parens_balance(lex_tok, lex_tok_count);
}

bool check_parens_balance(lex_token lex_tok[], int lex_tok_count) {
  stack s = {0};
  stack_init(&s);
  bool ret = true;

  for (int i = 0; i < lex_tok_count; i++) {
    switch (lex_tok[i]) {
      case LEX_EXPR_START:
      case LEX_PAREN_OPEN:
      case LEX_FILTER_START:
        /* todo: stack capacity check */
        stack_push(&s, &lex_tok[i]);
        break;
      case LEX_EXPR_END:
      case LEX_PAREN_CLOSE:
        if (s.size == 0) {
          ret = false;
          break;
        }

        lex_token* top = stack_top(&s);
        lex_token expected = (*top == LEX_PAREN_OPEN) ? LEX_PAREN_CLOSE : LEX_EXPR_END;
        if (lex_tok[i] != expected) {
          ret = false;
          break;
        }

        stack_pop(&s);
        break;
      default:
        /* not a paren, skip */
        break;
    }
  }

  if (!ret || s.size > 0) {
    zend_throw_exception(spl_ce_RuntimeException, "Query contains unbalanced parens/brackets", 0);
    return false;
  }

  return true;
}

void free_ast_nodes(struct ast_node* head) {
  if (head == NULL) {
    return;
  }

  free_ast_nodes(head->next);

  if (head->type == AST_EXPR) {
    free_ast_nodes(head->data.d_expression.head);
  } else if (head->type == AST_VALUE) {
    free_ast_nodes(head->data.d_value.head);
  }

  efree((void*)head);
}