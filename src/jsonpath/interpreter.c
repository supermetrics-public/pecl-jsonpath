#include "interpreter.h"

#include <ext/pcre/php_pcre.h>

#include "lexer.h"

int compare(zval* lh, zval* rh);
bool compare_rgxp(zval* lh, zval* rh);
void copy_result_to_return_value(zval* arr, zval* return_value);
bool evaluate_subexpression(zval* arr, enum ast_type operator_type, struct ast_node* lh_operand,
                            struct ast_node* rh_operand);
void exec_expression(zval* arr, struct ast_node* tok, zval* return_value);
void exec_index_filter(zval* arr, struct ast_node* tok, zval* return_value);
void exec_recursive_descent(zval* arr, struct ast_node* tok, zval* return_value);
void exec_selector(zval* arr, struct ast_node* tok, zval* return_value);
zval* exec_selector_iterative(zval* arr, struct ast_node* tok);
void exec_slice(zval* arr, struct ast_node* tok, zval* return_value);
void exec_wildcard(zval* arr, struct ast_node* tok, zval* return_value);
zval* operand_to_zval(struct ast_node* src, zval* tmp_dest, zval* arr);

void eval_ast(zval* arr, struct ast_node* tok, zval* return_value) {
  while (tok != NULL) {
    switch (tok->type) {
      case AST_INDEX_LIST:
        exec_index_filter(arr, tok, return_value);
        return;
      case AST_INDEX_SLICE:
        exec_slice(arr, tok, return_value);
        return;
      case AST_ROOT:
        tok = tok->next;
        break;
      case AST_RECURSE:
        tok = tok->next;
        exec_recursive_descent(arr, tok, return_value);
        return;
      case AST_SELECTOR:
        exec_selector(arr, tok, return_value);
        return;
      case AST_WILD_CARD:
        exec_wildcard(arr, tok, return_value);
        return;
      case AST_EXPR:
        exec_expression(arr, tok, return_value);
        return;
      default:
        assert(0);
        break;
    }
  }
}

void copy_result_to_return_value(zval* arr, zval* return_value) {
  zval tmp;
  ZVAL_COPY_VALUE(&tmp, arr);
  zval_copy_ctor(&tmp);
  add_next_index_zval(return_value, &tmp);
}

void exec_selector(zval* arr, struct ast_node* tok, zval* return_value) {
  if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
    return;
  }

  if ((arr = zend_hash_str_find(HASH_OF(arr), tok->data.d_selector.value, strlen(tok->data.d_selector.value))) ==
      NULL) {
    return;
  }

  if (tok->next != NULL) {
    eval_ast(arr, tok->next, return_value);
  } else {
    copy_result_to_return_value(arr, return_value);
  }
}

void exec_wildcard(zval* arr, struct ast_node* tok, zval* return_value) {
  if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
    return;
  }

  zval* data;
  zval* zv_dest;
  zend_string* key;
  zend_ulong num_key;

  ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
    if (tok->next == NULL) {
      copy_result_to_return_value(data, return_value);
    } else {
      eval_ast(data, tok->next, return_value);
    }
  }
  ZEND_HASH_FOREACH_END();
}

void exec_recursive_descent(zval* arr, struct ast_node* tok, zval* return_value) {
  if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
    return;
  }

  zval* data;
  zval* zv_dest;
  zend_string* key;
  zend_ulong num_key;

  eval_ast(arr, tok, return_value);

  ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) { exec_recursive_descent(data, tok, return_value); }
  ZEND_HASH_FOREACH_END();
}

void exec_index_filter(zval* arr, struct ast_node* tok, zval* return_value) {
  for (int i = 0; i < tok->data.d_list.count; i++) {
    if (tok->data.d_list.indexes[i] < 0) {
      tok->data.d_list.indexes[i] = zend_hash_num_elements(HASH_OF(arr)) - abs(tok->data.d_list.indexes[i]);
    }
    zval* data;
    if ((data = zend_hash_index_find(HASH_OF(arr), tok->data.d_list.indexes[i])) != NULL) {
      if (tok->next == NULL) {
        copy_result_to_return_value(data, return_value);
      } else {
        eval_ast(data, tok->next, return_value);
      }
    }
  }
}

void exec_slice(zval* arr, struct ast_node* tok, zval* return_value) {
  zval* data;

  int data_length = zend_hash_num_elements(HASH_OF(arr));

  int range_start = tok->data.d_list.indexes[0];
  int range_end = tok->data.d_list.count > 1 ? tok->data.d_list.indexes[1] : INT_MAX;
  int range_step = tok->data.d_list.count > 2 ? tok->data.d_list.indexes[2] : 1;

  // Zero-steps are not allowed, abort
  if (range_step == 0) {
    return;
  }

  // Replace placeholder with actual value
  if (range_start == INT_MAX) {
    range_start = range_step > 0 ? 0 : data_length - 1;
  }
  // Indexing from the end of the list
  else if (range_start < 0) {
    range_start = data_length - abs(range_start);
  }

  // Replace placeholder with actual value
  if (range_end == INT_MAX) {
    range_end = range_step > 0 ? data_length : -1;
  }
  // Indexing from the end of the list
  else if (range_end < 0) {
    range_end = data_length - abs(range_end);
  }

  // Set suitable boundaries for start index
  range_start = range_start < -1 ? -1 : range_start;
  range_start = range_start > data_length ? data_length : range_start;

  // Set suitable boundaries for end index
  range_end = range_end < -1 ? -1 : range_end;
  range_end = range_end > data_length ? data_length : range_end;

  if (range_step > 0) {
    // Make sure that the range is sane so we don't end up in an infinite loop
    if (range_start >= range_end) {
      return;
    }

    for (int i = range_start; i < range_end; i += range_step) {
      if ((data = zend_hash_index_find(HASH_OF(arr), i)) != NULL) {
        if (tok->next == NULL) {
          copy_result_to_return_value(data, return_value);
        } else {
          eval_ast(data, tok->next, return_value);
        }
      }
    }
  } else {
    // Make sure that the range is sane so we don't end up in an infinite loop
    if (range_start <= range_end) {
      return;
    }

    for (int i = range_start; i > range_end; i += range_step) {
      if ((data = zend_hash_index_find(HASH_OF(arr), i)) != NULL) {
        if (tok->next == NULL) {
          copy_result_to_return_value(data, return_value);
        } else {
          eval_ast(data, tok->next, return_value);
        }
      }
    }
  }
}

void exec_expression(zval* arr, struct ast_node* tok, zval* return_value) {
  zend_ulong num_key;
  zend_string* key;
  zval* data;

  ZEND_HASH_FOREACH_KEY_VAL(HASH_OF(arr), num_key, key, data) {
    if (evaluate_postfix_expression(data, tok->data.d_expression.head)) {
      if (tok->next == NULL) {
        copy_result_to_return_value(data, return_value);
      } else {
        eval_ast(data, tok->next, return_value);
      }
    }
  }
  ZEND_HASH_FOREACH_END();
}

/* This does the same thing as exec_selector_iterative except we don't */
/* continue recursing after a contiguous sequence of AST_SELECTOR has been */
/* read. */
zval* exec_selector_iterative(zval* arr, struct ast_node* tok) {
  if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
    return NULL;
  }

  zval* data;

  while (tok->type == AST_SELECTOR) {
    if ((data = zend_hash_str_find(HASH_OF(arr), tok->data.d_selector.value, strlen(tok->data.d_selector.value))) ==
        NULL) {
      return NULL;
    }
    arr = data;
    tok = tok->next;
  }

  return arr;
}

int compare(zval* lh, zval* rh) {
  zval result;
  ZVAL_NULL(&result);

  compare_function(&result, lh, rh);
  return (int)Z_LVAL(result);
}

bool compare_rgxp(zval* lh, zval* rh) {
  pcre_cache_entry* pce;

  if ((pce = pcre_get_compiled_regex_cache(Z_STR_P(rh))) == NULL) {
    zval_ptr_dtor(rh);
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

zval* operand_to_zval(struct ast_node* src, zval* tmp_dest, zval* arr) {
  if (src->type == AST_SELECTOR) {
    return exec_selector_iterative(arr, src);
  } else if (src->type == AST_LITERAL) {
    ZVAL_STRING(tmp_dest, src->data.d_literal.value);
    return tmp_dest;
  } else {
    /* todo: runtime error */
    return NULL;
  }
}

bool evaluate_subexpression(zval* arr, enum ast_type operator_type, struct ast_node* lh_operand,
                            struct ast_node* rh_operand) {
  switch (operator_type) {
    case AST_OR:
      return lh_operand->data.d_literal.value_bool || rh_operand->data.d_literal.value_bool;
    case AST_AND:
      return lh_operand->data.d_literal.value_bool && rh_operand->data.d_literal.value_bool;
    case AST_ISSET:
      return exec_selector_iterative(arr, lh_operand) != NULL;
    default:
      /* noop */
      break;
  }

  /* use stack-allocated zvals in order to avoid malloc, if possible */
  zval tmp_lh = {0}, tmp_rh = {0};

  zval* val_lh = operand_to_zval(lh_operand, &tmp_lh, arr);
  if (val_lh == NULL) {
    return false;
  }

  zval* val_rh = operand_to_zval(rh_operand, &tmp_rh, arr);
  if (val_rh == NULL) {
    return false;
  }

  bool ret;

  switch (operator_type) {
    case AST_EQ:
      ret = compare(val_lh, val_rh) == 0;
      break;
    case AST_NE:
      ret = compare(val_lh, val_rh) != 0;
      break;
    case AST_LT:
      ret = compare(val_lh, val_rh) < 0;
      break;
    case AST_LTE:
      ret = compare(val_lh, val_rh) <= 0;
      break;
    case AST_GT:
      ret = compare(val_lh, val_rh) > 0;
      break;
    case AST_GTE:
      ret = compare(val_lh, val_rh) >= 0;
      break;
    case AST_RGXP:
      ret = compare_rgxp(val_lh, val_rh);
      break;
    default:
      assert(0);
      break;
  }

  zval_ptr_dtor(val_lh);
  zval_ptr_dtor(val_rh);

  return ret;
}