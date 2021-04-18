#ifndef PARSER_H
#define PARSER_H 1

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "php.h"

#define PARSE_BUF_LEN 50

typedef enum {
  TYPE_OPERAND,
  TYPE_OPERATOR,
  TYPE_PAREN,
} operator_type;

enum ast_type {
  AST_AND,
  AST_BOOL,
  AST_EQ,
  AST_EXPR,
  AST_GT,
  AST_GTE,
  AST_INDEX_LIST,
  AST_INDEX_SLICE,
  AST_ISSET,
  AST_LITERAL_BOOL,
  AST_LITERAL,
  AST_LT,
  AST_LTE,
  AST_NE,
  AST_OR,
  AST_PAREN_LEFT,
  AST_PAREN_RIGHT,
  AST_RECURSE,
  AST_RGXP,
  AST_ROOT,
  AST_SELECTOR,
  AST_WILD_CARD,
  AST_HEAD
};

extern const char* AST_STR[];

union ast_node_data {
  struct {
    struct ast_node* head;
  } d_expression;
  struct {
    int count;
    int indexes[10]; /* todo check for max */
  } d_list;
  struct {
    char value[PARSE_BUF_LEN];
    bool value_bool;
  } d_literal;
  struct {
    char value[PARSE_BUF_LEN];
    bool child_scope; /* @.selector if true, else $.selector */
  } d_selector;
};

struct ast_node {
  struct ast_node* next;
  enum ast_type type;
  union ast_node_data data;
};

typedef struct {
  char msg[PARSE_BUF_LEN];
} parse_error;

bool evaluate_postfix_expression(zval* arr, struct ast_node* tok);

bool evaluate_subexpression(zval* array, enum ast_type operator_type, struct ast_node* lh_operand,
                            struct ast_node* rh_operand);

bool build_parse_tree(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                      int lex_tok_count, struct ast_node* head, parse_error* err);

bool check_parens_balance(lex_token lex_tok[], int lex_tok_count);

void free_ast_nodes(struct ast_node* head);

#endif /* PARSER_H */
