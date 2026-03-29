#include "deque.h"
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

bool deque_init(Deque **deque, void (*destroy)(void *)) {
    if (deque == nullptr)
        return false;
    *deque = nullptr;
    Deque *s = malloc(sizeof(*s));
    if (s == nullptr) {
        return false;
    }
    s->destroy = destroy;
    s->top = nullptr;
    s->tail = nullptr;
    s->count = 0;
    *deque = s;
    return true;
}

void deque_free(Deque *deque) {
    if (deque == nullptr)
        return;
    Node *n = deque->top;
    while (n != nullptr) {
        Node *next = n->next;
        if (deque->destroy != nullptr)
            deque->destroy(n->data);
        else
            free(n->data);

        free(n);
        n = next;
    }
    free(deque);
}

bool deque_push_back(Deque *deque, const void *data, size_t size) {
    if (deque == nullptr || data == nullptr || size == 0)
        return false;

    Node *n = create_node(data, size);
    if (n == nullptr)
        return false;

    n->prev = deque->tail;
    n->next = nullptr;

    if (deque->tail != nullptr)
        deque->tail->next = n;
    else
        deque->top = n;

    deque->tail = n;
    deque->count++;
    return true;
}

bool deque_pop_back(Deque *deque, void *data, size_t size_val, size_t *size) {
    if (deque == nullptr || data == nullptr || deque->tail == nullptr || size == nullptr)
        return false;

    Node *n = deque->tail;
    if (size_val < n->size)
        return false;

    memcpy(data, n->data, n->size);
    *size = n->size;
    Node *prev = n->prev;
    if (prev != nullptr)
        prev->next = nullptr;
    else
        deque->top = nullptr;

    deque->tail = prev;

    if (deque->destroy != nullptr)
        deque->destroy(n->data);
    else
        free(n->data);
    free(n);
    deque->count--;
    return true;
}

size_t deque_count(const Deque *deque) {
    if (deque == nullptr)
        return 0;
    return deque->count;
}

void deque_print_backward(const Deque *deque, void (*print)(const void *)) {
    if (deque == nullptr || print == nullptr)
        return;

    const Node *n = deque->tail;
    while (n != nullptr) {
        if (n->data != nullptr)
            print(n->data);
        n = n->prev;
    }
}

void deque_print_forward(const Deque *deque, void (*print)(const void *)) {
    if (deque == nullptr || print == nullptr)
        return;

    const Node *n = deque->top;
    while (n != nullptr) {
        if (n->data != nullptr)
            print(n->data);
        n = n->next;
    }
}

bool deque_push_front(Deque *deque, const void *data, size_t size) {
    if (deque == nullptr || data == nullptr || size == 0)
        return false;
    Node *n = create_node(data, size);
    if (n == nullptr)
        return false;
    Node *temp = deque->top;
    n->next = temp;
    n->prev = nullptr;
    if (deque->top != nullptr) {
        deque->top->prev = n;
    } else {
        deque->tail = n;
    }
    deque->top = n;
    deque->count++;
    return true;
}
bool deque_pop_front(Deque *deque, void *data, size_t size_val, size_t *size) {
    if (deque == nullptr || data == nullptr || deque->top == nullptr || size == nullptr)
        return false;
    Node *n = deque->top;
    if (size_val < n->size)
        return false;
    memcpy(data, n->data, n->size);
    *size = n->size;
    Node *next = n->next;
    if (next != nullptr)
        next->prev = nullptr;
    else
        deque->tail = nullptr;
    deque->top = next;
    if (deque->destroy != nullptr)
        deque->destroy(n->data);
    else
        free(n->data);
    free(n);
    deque->count--;
    return true;
}
bool deque_peek_front(const Deque *deque, void *data, size_t size) {
    if (deque == nullptr || data == nullptr || deque->top == nullptr)
        return false;
    Node *n = deque->top;
    if (size < n->size)
        return false;
    memcpy(data, n->data, n->size);
    return true;
}

bool deque_peek_back(const Deque *deque, void *data, size_t size) {
    if (deque == nullptr || data == nullptr || deque->tail == nullptr)
        return false;
    Node *n = deque->tail;
    if (size < n->size)
        return false;
    memcpy(data, n->data, n->size);
    return true;
}
