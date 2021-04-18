#ifndef STACK_H
#define STACK_H 1

// todo: enforce stack max check
#define STACK_MAX 100

struct stack {
  void* data[STACK_MAX];
  int size;
};

typedef struct stack stack;

void stack_init(stack*);
void* stack_top(stack*);
void stack_push(stack*, void*);
void stack_pop(stack*);

#endif /* STACK_H */
