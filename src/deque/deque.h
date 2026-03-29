#ifndef DEQUEUE_H
#define DEQUEUE_H

// implementation of Deque

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct Node_ {
    void *data;
    size_t size;
    struct Node_ *next, *prev;
} Node;

typedef struct Deque_ {
    Node *top, *tail;
    void (*destroy)(void *);
    size_t count;
} Deque;

bool deque_init(Deque **deque, void (*destroy)(void *));
void deque_print_backward(const Deque *deque, void (*print)(const void *));
void deque_print_forward(const Deque *deque, void (*print)(const void *));
void deque_free(Deque *deque);
bool deque_push_back(Deque *deque, const void *data, size_t size);
bool deque_push_front(Deque *deque, const void *data, size_t size);
bool deque_pop_back(Deque *deque, void *data, size_t size_val, size_t *size);
bool deque_pop_front(Deque *deque, void *data, size_t size_val, size_t *size);
bool deque_peek_front(const Deque *deque, void *data, size_t size);
bool deque_peek_back(const Deque *deque, void *data, size_t size);
size_t deque_count(const Deque *deque);

#endif
