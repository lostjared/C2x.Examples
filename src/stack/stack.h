#ifndef STACK_H
#define STACK_H

#include<string.h>
#include<stdio.h>
#include<stdlib.h>

typedef struct Node_ {
	void *data;
	size_t size;
	struct Node_ *next, *prev;
} Node;

typedef struct Stack_ {
	Node *top, *tail;
	void (*destroy)(void *);
} Stack;

bool stack_init(Stack **stack, void (*destroy)(void *));
void stack_print(Stack *stack, void (*print)(void *));
void stack_free(Stack *stack);
bool stack_push(Stack *stack, void *data, size_t size);
bool stack_pop(Stack *stack, void **data);
Node *node_create(void *data, size_t size);

#endif
