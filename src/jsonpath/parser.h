#ifndef PARSER_H
#define PARSER_H 1

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "zend_types.h"

#define NODE_POOL_LEN 64

enum ast_type {
  AST_AND,
  AST_BOOL,
  AST_CUR_NODE,
  AST_DOUBLE,
  AST_EQ,
  AST_EXPR,
  AST_GT,
  AST_GTE,
  AST_LITERAL,
  AST_LONG,
  AST_LT,
  AST_LTE,
  AST_NE,
  AST_NEGATION,
  AST_NULL,
  AST_OR,
  AST_RECURSE,
  AST_RGXP,
  AST_ROOT,
  AST_SELECTOR,
  AST_UNION_INDEX,
  AST_UNION_KEY,
  AST_UNION_SLICE,
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
    HashTable* ht;
  } d_list;
  struct {
    bool value_bool;
    zend_string* str;
  } d_literal;
  struct {
    char* val;
    int len;
  } d_selector;
  struct {
    double value;
  } d_double;
  struct {
    long value;
  } d_long;
  struct {
    struct ast_node* right;
  } d_unary;
};

struct ast_node {
  struct ast_node* next;
  enum ast_type type;
  union ast_node_data data;
};

struct node_pool {
  struct ast_node nodes[NODE_POOL_LEN];
  int cur_index;
};

bool is_binary(enum ast_type type);
bool is_unary(enum ast_type type);
struct ast_node* parse_jsonpath(struct jpath_token lex_tok[], int* lex_idx, int lex_tok_count, struct node_pool* pool);
void free_php_objects(struct node_pool* pool);

#ifdef JSONPATH_DEBUG
void print_ast(struct ast_node* head, const char* m, int level);
#endif

#endif /* PARSER_H */