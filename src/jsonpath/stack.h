#ifndef STACK_H
#define STACK_H 1

#include "lexer.h"
#include "parser.h"

#define STACK_MAX 100

struct stack {
    expr_operator *data[STACK_MAX];
    int size;
};

typedef struct stack stack;

void stack_init(stack *);
expr_operator *stack_top(stack *);
void stack_push(stack *, expr_operator *);
void stack_pop(stack *);

#endif				/* STACK_H */
