#include "stack.h"

#include <stddef.h>
#include <stdio.h>

void stack_init(stack* s) { s->size = 0; }

void* stack_top(stack* s) {
  if (s->size == 0) {
    printf("Stack is empty\n");
    return NULL;
  }

  return s->data[s->size - 1];
}

void stack_push(stack* s, void* tok) {
  if (s->size < STACK_MAX) {
    s->data[s->size++] = tok;
  } else {
    printf("Stack is empty\n");
  }
}

void stack_pop(stack* s) {
  if (s->size == 0) {
    printf("Stack is empty\n");
  } else {
    s->size--;
  }
}
