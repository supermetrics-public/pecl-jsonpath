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
operator_type get_token_type(enum ast_type);
struct ast_node* ast_alloc_node(struct ast_node* prev, enum ast_type type);
void delete_expression_head_node(struct ast_node* expr);
void parse_filter_list(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* start,
                       int lex_tok_count, struct ast_node* tok);
#ifdef JSONPATH_DEBUG
void print_ast(struct ast_node* head, const char* m, int level);
#endif
bool check_parens_balance(lex_token lex_tok[], int lex_tok_count);
bool validate_root_next(struct ast_node* head);

const char* AST_STR[] = {"AST_AND",     "AST_BOOL",       "AST_EQ",          "AST_EXPR",    "AST_GT",
                         "AST_GTE",     "AST_INDEX_LIST", "AST_INDEX_SLICE", "AST_ISSET",   "AST_LITERAL_BOOL",
                         "AST_LITERAL", "AST_LT",         "AST_LTE",         "AST_NE",      "AST_OPERAND",
                         "AST_OR",      "AST_PAREN_LEFT", "AST_PAREN_RIGHT", "AST_RECURSE", "AST_RGXP",
                         "AST_ROOT",    "AST_SELECTOR",   "AST_WILD_CARD",   "AST_HEAD"};

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
      case AST_EXPR:
        printf("\n");
        print_ast(head->data.d_expression.head, m, level + 1);
        break;
      case AST_OPERAND:
        printf("\n");
        print_ast(head->data.d_operand.head, m, level + 1);
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

        pfix = ast_alloc_node(pfix, AST_OPERAND);

        pfix->data.d_operand.head = cur;

        while (cur->next != NULL && get_token_type(cur->next->type) == TYPE_OPERAND) {
          cur = cur->next;
        }

        tmp = cur;
        cur = cur->next;
        tmp->next = NULL;
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

bool build_parse_tree(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                      int lex_tok_count, struct ast_node* head, parse_error* err) {
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

        if (*lex_idx < lex_tok_count - 1) { /* TODO next-node macro? */
          switch (lex_tok[(*lex_idx) + 1]) {
            case LEX_PAREN_CLOSE:
              /* fall-through */
            case LEX_OR:
              /* fall-through */
            case LEX_AND:
              cur = ast_alloc_node(cur, AST_ISSET);
              break;
            default:
              /* noop */
              break;
          }
        }
        break;
      case LEX_FILTER_START:

        if (*lex_idx == lex_tok_count - 1) { /* last token */
          strncpy(err->msg, "Missing filter end ]", sizeof(err->msg));
          return false;
        }

        (*lex_idx)++;

        switch (lex_tok[*lex_idx]) {
          case LEX_LITERAL:
            /* fall-through */
          case LEX_SLICE:
            /* fall-through */
          case LEX_CHILD_SEP:
            cur = ast_alloc_node(cur, AST_INDEX_LIST);
            parse_filter_list(lex_tok, lex_tok_values, lex_idx, lex_tok_count, cur);
            break;
          case LEX_WILD_CARD:
            cur = ast_alloc_node(cur, AST_WILD_CARD);
            break;
          case LEX_EXPR_END:
            strncpy(err->msg, "Filter must not be empty", sizeof(err->msg));
            return false;
          default:
            /* noop */
            break;
        }

        if (*lex_idx == lex_tok_count - 1 || lex_tok[(*lex_idx) + 1] != LEX_EXPR_END) { /* last token */
          strncpy(err->msg, "Missing filter end ]", sizeof(err->msg));
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
          strncpy(err->msg, "Buffer size exceeded", sizeof(err->msg));
          return false;
        }

        break;
      case LEX_LITERAL_BOOL:
        cur = ast_alloc_node(cur, AST_LITERAL_BOOL);

        if (jp_str_cpy(cur->data.d_literal.value, PARSE_BUF_LEN, lex_tok_values[*lex_idx],
                       strlen(lex_tok_values[*lex_idx])) > 0) {
          strncpy(err->msg, "Buffer size exceeded", sizeof(err->msg));
          return false;
        }

        if (strcmp(cur->data.d_literal.value, "true") == 0) {
          jp_str_cpy(cur->data.d_literal.value, PARSE_BUF_LEN, "JP_LITERAL_TRUE", 15);
        } else if (strcmp(cur->data.d_literal.value, "false") == 0) {
          jp_str_cpy(cur->data.d_literal.value, PARSE_BUF_LEN, "JP_LITERAL_FALSE", 16);
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

        build_parse_tree(lex_tok, lex_tok_values, lex_idx, lex_tok_count, cur->data.d_expression.head, err);

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
      default:
        break;
    }
    cur = cur->next;
  }

  return true;
}

void parse_filter_list(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                       int lex_tok_count, struct ast_node* tok) {
  int slice_count = 0;

  /* assume filter type is an index list by default. this resolves type */
  /* ambiguity of a filter containing no separators. */
  /* example: treat level4[0] as an index filter, not a slice. */
  tok->type = AST_INDEX_LIST;

  for (; *lex_idx < lex_tok_count; (*lex_idx)++) {
    if (lex_tok[*lex_idx] == LEX_EXPR_END) {
      /* lex_idx must point to LEX_EXPR_END after function returns */
      (*lex_idx)--;
      return;
    } else if (lex_tok[*lex_idx] == LEX_CHILD_SEP) {
      tok->type = AST_INDEX_LIST;
    } else if (lex_tok[*lex_idx] == LEX_SLICE) {
      tok->type = AST_INDEX_SLICE;

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
    } else if (lex_tok[*lex_idx] == LEX_LITERAL) {
      tok->data.d_list.indexes[tok->data.d_list.count] = atoi(lex_tok_values[*lex_idx]);
      tok->data.d_list.count++;
    } else {
      // todo error
    }
  }
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
    case AST_LITERAL:
    case AST_LITERAL_BOOL:
    case AST_BOOL:
    case AST_SELECTOR:
    case AST_OPERAND:
      return TYPE_OPERAND;
    default:
      assert(0);
      return TYPE_OPERATOR;
  }
}

bool evaluate_postfix_expression(zval* arr, struct ast_node* tok) {
  stack s;
  stack_init(&s);
  struct ast_node* expr_lh;
  struct ast_node* expr_rh;

  /* Temporary operators that store intermediate evaluations */
  struct ast_node op_true;

  op_true.type = AST_BOOL;
  op_true.data.d_literal.value_bool = true;

  struct ast_node op_false;

  op_false.type = AST_BOOL;
  op_false.data.d_literal.value_bool = false;

  while (tok != NULL) {
    switch (get_token_type(tok->type)) {
      case TYPE_OPERATOR:

        if (is_unary(tok->type)) {
          expr_rh = NULL;
          expr_lh = stack_top(&s);
        } else {
          expr_rh = stack_top(&s);
          stack_pop(&s);
          expr_lh = stack_top(&s);
        }

        stack_pop(&s);

        if (evaluate_subexpression(arr, tok->type, expr_lh->data.d_operand.head, expr_rh->data.d_operand.head)) {
          stack_push(&s, &op_true);
        } else {
          stack_push(&s, &op_false);
        }

        break;
      case TYPE_OPERAND:
        stack_push(&s, tok);
        /* TODO hack for now, place under value */
        if (tok->type == AST_SELECTOR) {
          while (tok->next != NULL && tok->next->type == AST_SELECTOR) {
            /* skip putting following selectors on stack */
            tok = tok->next;
          }
        }
        break;
      case TYPE_PAREN:
        /* there should be no parens in the postfix expression */
        assert(0);
    }

    tok = tok->next;
  }

  expr_lh = stack_top(&s);

  return expr_lh->data.d_literal.value_bool;
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
    case AST_LITERAL_BOOL:
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
  }

  efree((void*)head);
}