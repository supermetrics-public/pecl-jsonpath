#include "interpreter.h"

#include "exceptions.h"
#include "ext/pcre/php_pcre.h"
#include "lexer.h"

#define BOOL_ERR -1
#define RETURN_ERR_IF_NULL(val) \
  if (val == NULL) return -1
#define RETURN_NULL_IF_ERR(val) \
  if (val < 0) return NULL
#define BOOL_OR_ERR int

static BOOL_OR_ERR evaluate_binary(zval* arr_head, zval* arr_cur, struct ast_node* tok);
static BOOL_OR_ERR evaluate_expression(zval* arr_head, zval* arr_cur, struct ast_node* tok);
static BOOL_OR_ERR evaluate_unary(zval* arr_head, zval* arr_cur, struct ast_node* tok);
static bool break_if_result_found(zval* return_value);
static bool can_check_inequality(zval* lhs, zval* rhs);
static bool can_shortcut_binary_evaluation(struct ast_node* operator, zval * operand);
static bool compare_rgxp(zval* lh, zval* rh);
static int compare(zval* lh, zval* rh);
static void copy_result_or_continue(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_expression(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_index_filter(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_node_filter(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_recursive_descent(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_selector(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_slice(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static void exec_wildcard(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);
static zval* evaluate_primary(struct ast_node* src, zval* tmp_dest, zval* arr_head, zval* arr_cur);

void eval_ast(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (tok == NULL) {
    return;
  }
  switch (tok->type) {
    case AST_NODE_LIST:
      exec_node_filter(arr_head, arr_cur, tok, return_value);
      break;
    case AST_INDEX_LIST:
      exec_index_filter(arr_head, arr_cur, tok, return_value);
      break;
    case AST_INDEX_SLICE:
      exec_slice(arr_head, arr_cur, tok, return_value);
      break;
    case AST_CUR_NODE:
    case AST_ROOT:
      copy_result_or_continue(arr_head, arr_cur, tok, return_value);
      break;
    case AST_RECURSE:
      tok = tok->next;
      exec_recursive_descent(arr_head, arr_cur, tok, return_value);
      break;
    case AST_SELECTOR:
      exec_selector(arr_head, arr_cur, tok, return_value);
      break;
    case AST_WILD_CARD:
      exec_wildcard(arr_head, arr_cur, tok, return_value);
      break;
    case AST_EXPR:
      exec_expression(arr_head, arr_cur, tok, return_value);
      break;
    default:
      assert(0);
      break;
  }
}

static void exec_selector(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (arr_cur == NULL || Z_TYPE_P(arr_cur) != IS_ARRAY) {
    return;
  }

  zend_ulong idx;

  if (ZEND_HANDLE_NUMERIC_STR(tok->data.d_selector.val, tok->data.d_selector.len, idx)) {
    /* look up numeric index */
    arr_cur = zend_hash_index_find(HASH_OF(arr_cur), idx);
  } else {
    /* look up string index */
    arr_cur = zend_hash_str_find(HASH_OF(arr_cur), tok->data.d_selector.val, tok->data.d_selector.len);
  }

  if (arr_cur != NULL) {
    copy_result_or_continue(arr_head, arr_cur, tok, return_value);
  }
}

static void exec_wildcard(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (arr_cur == NULL || Z_TYPE_P(arr_cur) != IS_ARRAY) {
    return;
  }

  zval* data;

  ZEND_HASH_FOREACH_VAL_IND(HASH_OF(arr_cur), data) {
    copy_result_or_continue(arr_head, data, tok, return_value);
    if (break_if_result_found(return_value)) {
      break;
    }
  }
  ZEND_HASH_FOREACH_END();
}

static void exec_recursive_descent(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (arr_cur == NULL || Z_TYPE_P(arr_cur) != IS_ARRAY) {
    return;
  }

  eval_ast(arr_head, arr_cur, tok, return_value);

  zval* data;

  ZEND_HASH_FOREACH_VAL_IND(HASH_OF(arr_cur), data) {
    exec_recursive_descent(arr_head, data, tok, return_value);
  }
  ZEND_HASH_FOREACH_END();
}

static void exec_node_filter(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  zend_ulong idx;
  zval* data;

  ZEND_HASH_FOREACH_VAL(tok->data.d_list.ht, data) {
    if (ZEND_HANDLE_NUMERIC_STR(Z_STRVAL_P(data), Z_STRLEN_P(data), idx)) {
      /* look up numeric index */
      data = zend_hash_index_find(HASH_OF(arr_cur), idx);
    } else {
      /* look up string index */
      data = zend_hash_str_find(HASH_OF(arr_cur), Z_STRVAL_P(data), Z_STRLEN_P(data));
    }
    if (data != NULL) {
      copy_result_or_continue(arr_head, data, tok, return_value);
      if (break_if_result_found(return_value)) {
        break;
      }
    }
  }
  ZEND_HASH_FOREACH_END();
}

static void exec_index_filter(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (arr_cur == NULL || Z_TYPE_P(arr_cur) != IS_ARRAY) {
    return;
  }

  int i;

  for (i = 0; i < zend_hash_num_elements(tok->data.d_list.ht); i++) {
    int index = Z_LVAL_P(zend_hash_index_find(tok->data.d_list.ht, i));
    if (index < 0) {
      index = zend_hash_num_elements(HASH_OF(arr_cur)) - abs(index);
    }
    zval* data;
    if ((data = zend_hash_index_find(HASH_OF(arr_cur), index)) != NULL) {
      copy_result_or_continue(arr_head, data, tok, return_value);
      if (break_if_result_found(return_value)) {
        break;
      }
    }
  }
}

static void exec_slice(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (arr_cur == NULL || Z_TYPE_P(arr_cur) != IS_ARRAY) {
    return;
  }

  zval* data;
  int i;

  int data_length = zend_hash_num_elements(HASH_OF(arr_cur));
  int slice_length = zend_hash_num_elements(tok->data.d_list.ht);

  int range_start = Z_LVAL_P(zend_hash_index_find(tok->data.d_list.ht, 0));
  int range_end = slice_length > 1 ? (int)Z_LVAL_P(zend_hash_index_find(tok->data.d_list.ht, 1)) : INT_MAX;
  int range_step = slice_length > 2 ? (int)Z_LVAL_P(zend_hash_index_find(tok->data.d_list.ht, 2)) : 1;

  /* Zero-steps are not allowed, abort */
  if (range_step == 0) {
    return;
  }

  /* Replace placeholder with actual value */
  if (range_start == INT_MAX) {
    range_start = range_step > 0 ? 0 : data_length - 1;
  }
  /* Indexing from the end of the list */
  else if (range_start < 0) {
    range_start = data_length - abs(range_start);
  }

  /* Replace placeholder with actual value */
  if (range_end == INT_MAX) {
    range_end = range_step > 0 ? data_length : -1;
  }
  /* Indexing from the end of the list */
  else if (range_end < 0) {
    range_end = data_length - abs(range_end);
  }

  /* Set suitable boundaries for start index */
  range_start = range_start < -1 ? -1 : range_start;
  range_start = range_start > data_length ? data_length : range_start;

  /* Set suitable boundaries for end index */
  range_end = range_end < -1 ? -1 : range_end;
  range_end = range_end > data_length ? data_length : range_end;

  if (range_step > 0) {
    /* Make sure that the range is sane so we don't end up in an infinite loop */
    if (range_start >= range_end) {
      return;
    }

    for (i = range_start; i < range_end; i += range_step) {
      if ((data = zend_hash_index_find(HASH_OF(arr_cur), i)) != NULL) {
        copy_result_or_continue(arr_head, data, tok, return_value);
        if (break_if_result_found(return_value)) {
          break;
        }
      }
    }
  } else {
    /* Make sure that the range is sane so we don't end up in an infinite loop */
    if (range_start <= range_end) {
      return;
    }

    for (i = range_start; i > range_end; i += range_step) {
      if ((data = zend_hash_index_find(HASH_OF(arr_cur), i)) != NULL) {
        copy_result_or_continue(arr_head, data, tok, return_value);
        if (break_if_result_found(return_value)) {
          break;
        }
      }
    }
  }
}

static void exec_expression(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (arr_cur == NULL || Z_TYPE_P(arr_cur) != IS_ARRAY) {
    return;
  }

  zval* data;

  ZEND_HASH_FOREACH_VAL_IND(HASH_OF(arr_cur), data) {
    int res = evaluate_expression(arr_head, data, tok->data.d_expression.head);
    if (res == BOOL_ERR) {
      return;
    }
    if (res) {
      copy_result_or_continue(arr_head, data, tok, return_value);
      if (break_if_result_found(return_value)) {
        break;
      }
    }
  }
  ZEND_HASH_FOREACH_END();
}

int compare(zval* lh, zval* rh) {
  zval result;
  ZVAL_NULL(&result);

  compare_function(&result, lh, rh);
  return (int)Z_LVAL(result);
}

static bool compare_rgxp(zval* lh, zval* rh) {
  pcre_cache_entry* pce;

  if ((pce = pcre_get_compiled_regex_cache(Z_STR_P(rh))) == NULL) {
    throw_jsonpath_exception("Invalid regex pattern `%s`", Z_STRVAL_P(rh));
    return false;
  }

  zval retval;
  zval subpats;

  ZVAL_NULL(&retval);
  ZVAL_NULL(&subpats);

  zend_string* s_lh = zend_string_copy(Z_STR_P(lh));

  php_pcre_match_impl(pce, s_lh, &retval, &subpats, 0, 0, 0, 0);

  zend_string_release_ex(s_lh, 0);
  zval_ptr_dtor(&subpats);

  return Z_LVAL(retval) > 0;
}

static zval* evaluate_primary(struct ast_node* src, zval* tmp_dest, zval* arr_head, zval* arr_cur) {
  switch (src->type) {
    case AST_BOOL:
      ZVAL_BOOL(tmp_dest, src->data.d_literal.value_bool);
      return tmp_dest;
    case AST_DOUBLE:
      ZVAL_DOUBLE(tmp_dest, src->data.d_double.value);
      return tmp_dest;
    case AST_LITERAL:
      ZVAL_NEW_STR(tmp_dest, src->data.d_literal.str);
      return tmp_dest;
    case AST_LONG:
      ZVAL_LONG(tmp_dest, src->data.d_long.value);
      return tmp_dest;
    case AST_NULL:
      ZVAL_NULL(tmp_dest);
      return tmp_dest;
    case AST_ROOT:
      ZVAL_INDIRECT(tmp_dest, NULL);
      eval_ast(arr_head, arr_head, src, tmp_dest);
      if (Z_INDIRECT_P(tmp_dest) == NULL) {
        ZVAL_UNDEF(tmp_dest);
        return tmp_dest;
      }
      return Z_INDIRECT_P(tmp_dest);
    case AST_CUR_NODE:
    case AST_SELECTOR:
      ZVAL_INDIRECT(tmp_dest, NULL);
      eval_ast(arr_head, arr_cur, src, tmp_dest);
      if (Z_INDIRECT_P(tmp_dest) == NULL) {
        ZVAL_UNDEF(tmp_dest);
        return tmp_dest;
      }
      return Z_INDIRECT_P(tmp_dest);
    case AST_NODE_LIST:
      ZVAL_ARR(tmp_dest, src->data.d_list.ht);
      return tmp_dest;
    default:
      throw_jsonpath_exception("Unsupported expression operand");
      return NULL;
  }
}

static bool break_if_result_found(zval* return_value) {
  return Z_TYPE_P(return_value) == IS_INDIRECT && Z_INDIRECT_P(return_value) != NULL;
}

static void copy_result_or_continue(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value) {
  if (tok->next == NULL) {
    if (Z_TYPE_P(return_value) == IS_ARRAY) {
      zval tmp;
      ZVAL_COPY_VALUE(&tmp, arr_cur);
      zval_copy_ctor(&tmp);
      add_next_index_zval(return_value, &tmp);
    } else if (Z_TYPE_P(return_value) == IS_INDIRECT) {
      ZVAL_INDIRECT(return_value, arr_cur);
    }
  } else {
    eval_ast(arr_head, arr_cur, tok->next, return_value);
  }
}

static BOOL_OR_ERR evaluate_expression(zval* arr_head, zval* arr_cur, struct ast_node* tok) {
  if (is_binary(tok->type)) {
    return evaluate_binary(arr_head, arr_cur, tok);
  }

  if (is_unary(tok->type)) {
    return evaluate_unary(arr_head, arr_cur, tok);
  }

  if (tok->type == AST_SELECTOR || tok->type == AST_CUR_NODE) {
    zval tmp = {0};
    zval* res = evaluate_primary(tok, &tmp, arr_head, arr_cur);
    RETURN_ERR_IF_NULL(res);
    return Z_TYPE_P(res) != IS_UNDEF;
  }

  zval tmp = {0};
  zval* val = evaluate_primary(tok->data.d_unary.right, &tmp, arr_head, arr_cur);

  RETURN_ERR_IF_NULL(val);

  if (Z_TYPE_P(val) == IS_FALSE) {
    return true;
  }

  return false;
}

static BOOL_OR_ERR evaluate_unary(zval* arr_head, zval* arr_cur, struct ast_node* tok) {
  zval tmp = {0};

  if (is_unary(tok->data.d_unary.right->type)) {
    return !evaluate_unary(arr_head, arr_cur, tok->data.d_unary.right);
  } else if (is_binary(tok->data.d_unary.right->type)) {
    return !evaluate_binary(arr_head, arr_cur, tok->data.d_unary.right);
  } else if (tok->data.d_unary.right->type == AST_SELECTOR || tok->data.d_unary.right->type == AST_CUR_NODE) {
    /* ?(!@.selector) */
    zval* res = evaluate_primary(tok->data.d_unary.right, &tmp, arr_head, arr_cur);
    RETURN_ERR_IF_NULL(res);
    return Z_TYPE_P(res) == IS_UNDEF;
  }

  zval* val = evaluate_primary(tok->data.d_unary.right, &tmp, arr_head, arr_cur);

  RETURN_ERR_IF_NULL(val);

  if (Z_TYPE_P(val) == IS_FALSE) {
    return true;
  }

  return false;
}

zval* evaluate_operand(zval* arr_head, zval* arr_cur, struct ast_node* tok, struct ast_node* operand, zval* tmp_val) {
  if (is_binary(operand->type)) {
    int result = evaluate_binary(arr_head, arr_cur, operand);
    RETURN_NULL_IF_ERR(result);
    ZVAL_BOOL(tmp_val, result);
    return tmp_val;
  } else if (is_unary(tok->type)) {
    int result = evaluate_unary(arr_head, arr_cur, operand);
    RETURN_NULL_IF_ERR(result);
    ZVAL_BOOL(tmp_val, result);
    return tmp_val;
  } else if ((operand->type == AST_SELECTOR || operand->type == AST_CUR_NODE) &&
             (tok->type == AST_OR || tok->type == AST_AND)) {
    /* ?(@.selector <or|and> [operand]) */
    zval* tmp = evaluate_primary(operand, tmp_val, arr_head, arr_cur);
    if (tmp == NULL) {
      return NULL;
    }
    bool exists = Z_TYPE_P(tmp) != IS_UNDEF;
    ZVAL_BOOL(tmp_val, exists);
    return tmp_val;
  }

  return evaluate_primary(operand, tmp_val, arr_head, arr_cur);
}

/* Only != operator can support undefined operand values (i.e. when a selector returns no result). This check is mostly
 * an optimization, but is necessary to prevent compare_rgxp() from segfaulting on undefined values. */
static bool can_shortcut_binary_evaluation(struct ast_node* operator, zval * operand) {
  return Z_TYPE_P(operand) == IS_UNDEF && operator->type != AST_NE;
}

static BOOL_OR_ERR evaluate_binary(zval* arr_head, zval* arr_cur, struct ast_node* tok) {
  /* use stack-allocated zvals in order to avoid malloc, if possible */
  zval tmp_lh = {0}, tmp_rh = {0};
  struct ast_node *lh_operand = tok->data.d_binary.left, *rh_operand = tok->data.d_binary.right;

  zval* val_lh = evaluate_operand(arr_head, arr_cur, tok, lh_operand, &tmp_lh);

  RETURN_ERR_IF_NULL(val_lh);

  if (can_shortcut_binary_evaluation(tok, val_lh)) {
    return false;
  }

  zval* val_rh = evaluate_operand(arr_head, arr_cur, tok, rh_operand, &tmp_rh);

  if (val_rh == NULL) {
    return BOOL_ERR;
  }

  if (can_shortcut_binary_evaluation(tok, val_rh)) {
    return false;
  }

  switch (tok->type) {
    case AST_EQ:
      return fast_is_identical_function(val_lh, val_rh);
    case AST_NE:
      return !fast_is_identical_function(val_lh, val_rh);
    case AST_LT:
      return can_check_inequality(val_lh, val_rh) && compare(val_lh, val_rh) < 0;
    case AST_LTE:
      return can_check_inequality(val_lh, val_rh) && compare(val_lh, val_rh) <= 0;
    case AST_OR:
      return (Z_TYPE_P(val_lh) == IS_TRUE) || (Z_TYPE_P(val_rh) == IS_TRUE);
    case AST_AND:
      return (Z_TYPE_P(val_lh) == IS_TRUE) && (Z_TYPE_P(val_rh) == IS_TRUE);
    case AST_GT:
      return can_check_inequality(val_lh, val_rh) && compare(val_lh, val_rh) > 0;
    case AST_GTE:
      return can_check_inequality(val_lh, val_rh) && compare(val_lh, val_rh) >= 0;
    case AST_RGXP:
      return compare_rgxp(val_lh, val_rh);
    default:
      assert(0);
      return false;
  }
}

/* Determine if two zvals can be checked for inequality (>, <, >=, <=). Specifically forbid comparing strings with
 * numeric values in order to avoid returning true for scenarios such as 42 > "value". */
static bool can_check_inequality(zval* lhs, zval* rhs) {
  bool lhs_is_numeric = (Z_TYPE_P(lhs) == IS_LONG || Z_TYPE_P(lhs) == IS_DOUBLE);
  bool rhs_is_numeric = (Z_TYPE_P(rhs) == IS_LONG || Z_TYPE_P(rhs) == IS_DOUBLE);

  if (lhs_is_numeric && rhs_is_numeric) {
    return true;
  }

  bool lhs_is_string = Z_TYPE_P(lhs) == IS_STRING;
  bool rhs_is_string = Z_TYPE_P(rhs) == IS_STRING;

  return lhs_is_string && rhs_is_string;
}