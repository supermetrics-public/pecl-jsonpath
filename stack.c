#include "stack.h"
#include "expr.h"
#include <stdio.h>
#include <stdlib.h>


void Stack_Init(Stack *S)
{
    S->size = 0;
}

expr * Stack_Top(Stack *S)
{
    if (S->size == 0) {
        fprintf(stderr, "Error: stack empty\n");
        return NULL;
    }

    return S->data[S->size-1];
}

void Stack_Push(Stack *S, expr * expr)
{
    if (S->size < STACK_MAX)
        S->data[S->size++] = expr;
    else
        fprintf(stderr, "Error: stack full\n");
}

void Stack_Pop(Stack *S)
{
    if (S->size == 0)
        fprintf(stderr, "Error: stack empty\n");
    else
        S->size--;
}