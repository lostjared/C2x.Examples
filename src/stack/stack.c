#include "stack.h"
#include <stdio.h>
#include <string.h>

Node *create_node(const void *data, size_t size) {
    if (data == nullptr || size == 0)
        return nullptr;
    Node *n = nullptr;
    if ((n = malloc(sizeof(*n))) != nullptr) {
        n->size = size;
        n->data = malloc(size);
        if (n->data == nullptr) {
            free(n);
            return nullptr;
        }
        memcpy(n->data, data, size);
        n->next = nullptr;
        n->prev = nullptr;
    }
    return n;
}

bool stack_init(Stack **stack, void (*destroy)(void *)) {
    if (stack == nullptr)
        return false;

    Stack *s = malloc(sizeof(*s));
    if (s == nullptr) {
        return false;
    }
    s->destroy = destroy;
    s->top = nullptr;
    s->tail = nullptr;
    *stack = s;
    return true;
}

void stack_free(Stack *stack) {
    if (stack == nullptr)
        return;
    Node *n = stack->top;
    while (n != nullptr) {
        Node *next = n->next;
        if (stack->destroy != nullptr)
            stack->destroy(n->data);
        else
            free(n->data);
        free(n);
        n = next;
    }
    free(stack);
}

bool stack_push(Stack *stack, const void *data, size_t size) {
    if (stack == nullptr || data == nullptr || size == 0)
        return false;

    Node *n = create_node(data, size);
    if (n == nullptr)
        return false;

    n->prev = stack->tail;
    n->next = nullptr;

    if (stack->tail != nullptr)
        stack->tail->next = n;
    else
        stack->top = n;

    stack->tail = n;
    return true;
}

bool stack_pop(Stack *stack, void **data, size_t *size) {
    if (stack == nullptr || data == nullptr || stack->tail == nullptr || size == nullptr)
        return false;

    Node *n = stack->tail;
    *data = malloc(n->size);
    if (*data == nullptr)
        return false;

    memcpy(*data, n->data, n->size);
    *size = n->size;
    Node *prev = n->prev;
    if (prev != nullptr)
        prev->next = nullptr;
    else
        stack->top = nullptr;

    stack->tail = prev;

    if (stack->destroy != nullptr)
        stack->destroy(n->data);
    else
        free(n->data);
    free(n);
    return true;
}

void stack_print(Stack *stack, void (*print)(void *)) {
    if (stack == nullptr || print == nullptr)
        return;

    Node *n = stack->tail;
    while (n != nullptr) {
        if (n->data != nullptr)
            print(n->data);
        n = n->prev;
    }
}
