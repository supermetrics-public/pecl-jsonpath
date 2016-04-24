#ifndef STACK_H
#define STACK_H 1

#include "expr.h"

#define STACK_MAX 100


struct Stack {
    expr     * data[STACK_MAX];
    int     size;
};
typedef struct Stack Stack;


void Stack_Init(Stack *S);
expr * Stack_Top(Stack *S);
void Stack_Push(Stack *S, expr * expr);
void Stack_Pop(Stack *S);

#endif /* STACK_H */