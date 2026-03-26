#include "dequeue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *create_node(const void *data, size_t size) {
    if (data == nullptr || size == 0)
        return nullptr;
    Node *n = nullptr;
    n = malloc(sizeof(*n));
    if (n == nullptr)
        return nullptr;

    n->size = size;
    n->data = malloc(size);
    if (n->data == nullptr) {
        free(n);
        return nullptr;
    }
    memcpy(n->data, data, size);
    n->next = nullptr;
    n->prev = nullptr;
    return n;
}

bool dequeue_init(Dequeue **dequeue, void (*destroy)(void *)) {
    if (dequeue == nullptr)
        return false;
    *dequeue = nullptr;
    Dequeue *s = malloc(sizeof(*s));
    if (s == nullptr) {
        return false;
    }
    s->destroy = destroy;
    s->top = nullptr;
    s->tail = nullptr;
    s->count = 0;
    *dequeue = s;
    return true;
}

void dequeue_free(Dequeue *dequeue) {
    if (dequeue == nullptr)
        return;
    Node *n = dequeue->top;
    while (n != nullptr) {
        Node *next = n->next;
        if (dequeue->destroy != nullptr)
            dequeue->destroy(n->data);
        else
            free(n->data);
        free(n);
        n = next;
    }
    free(dequeue);
}

bool dequeue_push_back(Dequeue *dequeue, const void *data, size_t size) {
    if (dequeue == nullptr || data == nullptr || size == 0)
        return false;

    Node *n = create_node(data, size);
    if (n == nullptr)
        return false;

    n->prev = dequeue->tail;
    n->next = nullptr;

    if (dequeue->tail != nullptr)
        dequeue->tail->next = n;
    else
        dequeue->top = n;

    dequeue->tail = n;
    dequeue->count++;
    return true;
}

bool dequeue_pop_back(Dequeue *dequeue, void *data, size_t size_val, size_t *size) {
    if (dequeue == nullptr || data == nullptr || dequeue->tail == nullptr || size == nullptr)
        return false;

    Node *n = dequeue->tail;
    if (size_val < n->size)
        return false;

    memcpy(data, n->data, n->size);
    *size = n->size;
    Node *prev = n->prev;
    if (prev != nullptr)
        prev->next = nullptr;
    else
        dequeue->top = nullptr;

    dequeue->tail = prev;

    if (dequeue->destroy != nullptr)
        dequeue->destroy(n->data);
    else
        free(n->data);
    free(n);
    dequeue->count--;
    return true;
}

size_t dequeue_count(const Dequeue *dequeue) {
    if (dequeue == nullptr)
        return 0;
    return dequeue->count;
}

void dequeue_print_backward(const Dequeue *dequeue, void (*print)(const void *)) {
    if (dequeue == nullptr || print == nullptr)
        return;

    const Node *n = dequeue->tail;
    while (n != nullptr) {
        if (n->data != nullptr)
            print(n->data);
        n = n->prev;
    }
}

void dequeue_print_forward(const Dequeue *dequeue, void (*print)(const void *)) {
    if (dequeue == nullptr || print == nullptr)
        return;

    const Node *n = dequeue->top;
    while (n != nullptr) {
        if (n->data != nullptr)
            print(n->data);
        n = n->next;
    }
}

bool dequeue_push_front(Dequeue *dequeue, const void *data, size_t size) {
    if (dequeue == nullptr || data == nullptr || size == 0)
        return false;
    Node *n = create_node(data, size);
    if (n == nullptr)
        return false;
    Node *temp = dequeue->top;
    n->next = temp;
    n->prev = nullptr;
    if (dequeue->top != nullptr) {
        dequeue->top->prev = n;
    } else {
        dequeue->tail = n;
    }
    dequeue->top = n;
    dequeue->count++;
    return true;
}
bool dequeue_pop_front(Dequeue *dequeue, void *data, size_t size_val, size_t *size) {
    if (dequeue == nullptr || data == nullptr || dequeue->top == nullptr || size == nullptr)
        return false;
    Node *n = dequeue->top;
    if (size_val < n->size)
        return false;
    memcpy(data, n->data, n->size);
    *size = n->size;
    Node *next = n->next;
    if (next != nullptr)
        next->prev = nullptr;
    else
        dequeue->tail = nullptr;
    dequeue->top = next;
    /* do NOT destroy or free n->data here; caller now owns the popped payload resources */
    free(n->data);
    free(n);
    dequeue->count--;
    return true;
}
bool dequeue_peek_front(const Dequeue *dequeue, void *data, size_t size) {
    if (dequeue == nullptr || data == nullptr || dequeue->top == nullptr)
        return false;
    Node *n = dequeue->top;
    if (size < n->size)
        return false;
    memcpy(data, n->data, n->size);
    return true;
}

bool dequeue_peek_back(const Dequeue *dequeue, void *data, size_t size) {
    if (dequeue == nullptr || data == nullptr || dequeue->tail == nullptr)
        return false;
    Node *n = dequeue->tail;
    if (size < n->size)
        return false;
    memcpy(data, n->data, n->size);
    return true;
}
