#ifndef PARSER_H
#define PARSER_H 1

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define PARSE_BUF_LEN 50

typedef enum {
  TYPE_OPERAND,
  TYPE_OPERATOR,
  TYPE_PAREN,
} operator_type;

enum ast_type {
  AST_AND,
  AST_BOOL,
  AST_DOUBLE,
  AST_EQ,
  AST_EXPR,
  AST_GT,
  AST_GTE,
  AST_INDEX_LIST,
  AST_INDEX_SLICE,
  AST_LITERAL,
  AST_LONG,
  AST_LT,
  AST_LTE,
  AST_NE,
  AST_NULL,
  AST_OR,
  AST_PAREN_LEFT,
  AST_PAREN_RIGHT,
  AST_RECURSE,
  AST_RGXP,
  AST_ROOT,
  AST_SELECTOR,
  AST_WILD_CARD
};

extern const char* AST_STR[];

union ast_node_data {
  struct {
    struct ast_node* left;
    struct ast_node* right;
  } d_binary;
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
  } d_selector;
  struct {
    struct ast_node* head;
  } d_value;
  struct {
    double value;
  } d_double;
  struct {
    long value;
  } d_long;
};

struct ast_node {
  struct ast_node* next;
  enum ast_type type;
  union ast_node_data data;
};

bool build_parse_tree(lex_token lex_tok[PARSE_BUF_LEN], char lex_tok_values[][PARSE_BUF_LEN], int* lex_idx,
                      int lex_tok_count, struct ast_node* head);
bool sanity_check(lex_token lex_tok[], int lex_tok_count);
void free_ast_nodes(struct ast_node* head);
bool is_binary(enum ast_type type);
bool validate_parse_tree(struct ast_node* head);

#ifdef JSONPATH_DEBUG
void print_ast(struct ast_node* head, const char* m, int level);
#endif

#endif /* PARSER_H */
