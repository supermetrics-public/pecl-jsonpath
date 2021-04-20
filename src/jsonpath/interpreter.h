#ifndef INTERPRETER_H
#define INTERPRETER_H 1

#include "parser.h"
#include "php.h"

void eval_ast(zval* arr_head, zval* arr_cur, struct ast_node* tok, zval* return_value);

#endif /* INTERPRETER_H */