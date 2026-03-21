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

typedef struct Queue_ {
	Node *top, *tail;
	void (*destroy)(void *);
	size_t count;
} Queue;

bool queue_init(Queue **queue, void (*destroy)(void *));
void queue_print(Queue *queue, void (*print)(void *));
void queue_free(Queue *queue);
bool queue_push_back(Queue *queue, const void *data, size_t size);
bool queue_push_front(Queue *queue, const void *data, size_t size);
bool queue_pop(Queue *queue, void **data, size_t *size);
bool queue_pop_front(Queue *queue, void **data, size_t *size);
bool queue_peek_front(Queue *queue, void *data, size_t size);
bool queue_peek_back(Queue *queue, void *data, size_t size);
Node *create_node(const void *data, size_t size);


#endif
