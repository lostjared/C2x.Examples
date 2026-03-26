#ifndef DEQUEUE_H
#define DEQUEUE_H

// implementation of Dequeue

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct Node_ {
    void *data;
    size_t size;
    struct Node_ *next, *prev;
} Node;

typedef struct Dequeue_ {
    Node *top, *tail;
    void (*destroy)(void *);
    size_t count;
} Dequeue;

bool dequeue_init(Dequeue **dequeue, void (*destroy)(void *));
void dequeue_print_backward(const Dequeue *dequeue, void (*print)(const void *));
void dequeue_print_forward(const Dequeue *dequeue, void (*print)(const void *));
void dequeue_free(Dequeue *dequeue);
bool dequeue_push_back(Dequeue *dequeue, const void *data, size_t size);
bool dequeue_push_front(Dequeue *dequeue, const void *data, size_t size);
bool dequeue_pop_back(Dequeue *dequeue, void *data, size_t size_val, size_t *size);
bool dequeue_pop_front(Dequeue *dequeue, void *data, size_t size_val, size_t *size);
bool dequeue_peek_front(const Dequeue *dequeue, void *data, size_t size);
bool dequeue_peek_back(const Dequeue *dequeue, void *data, size_t size);
size_t dequeue_count(const Dequeue *dequeue);

#endif
