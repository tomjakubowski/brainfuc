#include "stack.h"

void stack_push(stack **stack_ptr, stack_element data) {
    stack *head = malloc(sizeof(head));
    head->next = *stack_ptr;
    head->data = data;
    *stack_ptr = head;
}

stack_element stack_pop(stack **stack_ptr) {
    stack *head = *stack_ptr;
    if (stack_empty(head)) {
        fputs("Error: stack underflow\n", stderr);
        abort();
    }

    stack_element data = head->data;
    *stack_ptr = head->next;
    free(head);
    return data;
}

bool stack_empty(stack *stack_ptr) {
    return (stack_ptr == NULL);
}
