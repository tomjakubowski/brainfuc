#ifndef BRAINFUC_STACK_H
#define BRAINFUC_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef char * stack_element;
typedef struct stack {
    struct stack *next;
    stack_element data;
} stack;

void stack_push(stack **, stack_element);
stack_element stack_pop(stack **);
bool stack_empty(stack *);

#endif
