#include"dequeue.h"
#include<string.h>
#include<stdio.h>

Node *create_node(const void *data, size_t size) {
	if(data == nullptr || size == 0) return nullptr;
	Node *n = nullptr;
	if((n = malloc(sizeof(*n) )) != nullptr) {
		n->size = size;
		n->data = malloc(size);
		if(n->data == nullptr) {
			free(n);
			return nullptr;
		}
		memcpy(n->data, data, size);
		n->next = nullptr;
	        n->prev = nullptr;	
	} 
	return n;

}

bool queue_init(Queue **queue, void (*destroy)(void *)) {
	if(queue == nullptr)
		return false;

	Queue *s = malloc(sizeof (*s));
	if(s == nullptr) {
		return false;
	}
	s->destroy = destroy;
	s->top = nullptr;
	s->tail = nullptr;
	s->count = 0;
	*queue = s;
	return true;
}

void queue_free(Queue *queue) {
	if(queue == nullptr)
		return;
	Node *n = queue->top;
	while(n != nullptr) {
		Node *next = n->next;
		if(queue->destroy != nullptr)
			queue->destroy(n->data);
		else
			free(n->data);
		free(n);
		n = next;
	}
	free(queue);
}

bool queue_push_back(Queue *queue, const void *data, size_t size) {
    if(queue == nullptr || data == nullptr || size == 0)
        return false;

    Node *n = create_node(data, size);
    if(n == nullptr)
        return false;

    n->prev = queue->tail;
    n->next = nullptr;

    if(queue->tail != nullptr)
        queue->tail->next = n;
    else
        queue->top = n;

    queue->tail = n;
	queue->count++;
    return true;
}

bool queue_pop(Queue *queue, void **data, size_t *size) {
    if(queue == nullptr || data == nullptr || queue->tail == nullptr || size == nullptr)
        return false;

    Node *n = queue->tail;
    *data = malloc(n->size);
    if(*data == nullptr)
        return false;

    memcpy(*data, n->data, n->size);
    *size = n->size;
    Node *prev = n->prev;
    if(prev != nullptr)
        prev->next = nullptr;
    else
        queue->top = nullptr;

    queue->tail = prev;

    if(queue->destroy != nullptr)
	queue->destroy(n->data);
    else
    	free(n->data);
    free(n);
	queue->count--;
    return true;
}

void queue_print(Queue *queue, void (*print)(void *)) {
	if(queue == nullptr || print == nullptr) 
		return;

	Node *n = queue->tail;
	while(n != nullptr) {
		if(n->data != nullptr)
			print(n->data);
		n = n->prev;
	}
}

bool queue_push_front(Queue *queue, const void *data, size_t size) {
	if(queue == nullptr || data == nullptr || size == 0)
        return false;
	Node *n = create_node(data, size);
	if(n == nullptr) 
		return false;
	Node *temp = queue->top;
	n->next = temp;
	n->prev = nullptr;
	if(queue->top != nullptr) {
		queue->top->prev = n;
	} else {
		queue->tail = n;
	}
	queue->top = n;
	queue->count++;
	return true;
}
bool queue_pop_front(Queue *queue, void **data, size_t *size) {
	if(queue == nullptr || data == nullptr || queue->tail == nullptr || size == nullptr)
        return false;
    Node *n = queue->top;
    *data = malloc(n->size);
    if(*data == nullptr)
        return false;

    memcpy(*data, n->data, n->size);
    *size = n->size;
    Node *next = n->next;
    if(next != nullptr)
     	next->prev = nullptr;
    else
        queue->tail = nullptr;

    queue->top = next;
    if(queue->destroy != nullptr)
	queue->destroy(n->data);
    else
    	free(n->data);
    free(n);
	queue->count--;
    return true;
}
bool queue_peek_front(Queue *queue, void *data, size_t size) {
	if(queue == nullptr || data == nullptr || queue->top == nullptr)
        return false;
    Node *n = queue->top;
	if(n == nullptr || size < n->size)
		return false;
	memcpy(data, n->data, n->size);
    return true;
}

bool queue_peek_back(Queue *queue, void *data,  size_t size) {
	if(queue == nullptr || data == nullptr || queue->tail == nullptr)
        return false;
    Node *n = queue->tail;
	if(n == nullptr || size < n->size)
		return false;
    memcpy(data, n->data, n->size);
    return true;
}