#include "stack.h"
#include <stdio.h>

void stack_init(stack * s)
{
    s->size = 0;
}

expr_operator *stack_top(stack * s)
{
    if (s->size == 0) {
	printf("Stack is empty\n");
	return NULL;
    }

    return s->data[s->size - 1];
}

void stack_push(stack * s, expr_operator * expr)
{
    if (s->size < STACK_MAX) {
	s->data[s->size++] = expr;
    } else {
	printf("Stack is empty\n");
    }
}

void stack_pop(stack * s)
{
    if (s->size == 0) {
	printf("Stack is empty\n");
    } else {
	s->size--;
    }
}
