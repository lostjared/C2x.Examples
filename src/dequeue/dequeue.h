#ifndef DEQUEUE_H
#define DEQUEUE_H

// implementation of Dequeue

#include<stdlib.h>
#include<stddef.h>
#include<stdbool.h>

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
void dequeue_print_backward(const Dequeue *dequeue, void (*print)(void *));
void dequeue_print_forward(const Dequeue *dequeue, void (*print)(void *));
void dequeue_free(Dequeue *dequeue);
bool dequeue_push_back(Dequeue *dequeue, const void *data, size_t size);
bool dequeue_push_front(Dequeue *dequeue, const void *data, size_t size);
bool dequeue_pop_back(Dequeue *dequeue, void *data, size_t *size);
bool dequeue_pop_front(Dequeue *dequeue, void *data, size_t *size);
bool dequeue_peek_front(const Dequeue *dequeue, void *data, size_t size);
bool dequeue_peek_back(const Dequeue *dequeue, void *data, size_t size);
Node *create_node(const void *data, size_t size);

#endif
