#include "deque.h"
#include <stdio.h>
#include <stdlib.h>

void release(void *ptr) {
    free(ptr);
}

void echo(const void *p) {
    const size_t *v = p;
    printf("\tValue is: %zu\n", *v);
}

int main(void) {
    Deque *deque = nullptr;
    if (!deque_init(&deque, release)) {
        fprintf(stderr, "Error setting up deque.\n");
        return EXIT_FAILURE;
    }
    size_t value = 255;
    if (!deque_push_front(deque, &value, sizeof(value))) {
        fprintf(stderr, "Error pushing into deque.\n");
        deque_free(deque);
        return EXIT_FAILURE;
    }
    value = 100;
    if (!deque_push_front(deque, &value, sizeof(value))) {
        fprintf(stderr, "Error pushing into deque\n");
        deque_free(deque);
        return EXIT_FAILURE;
    }
    printf("backward {\n");
    deque_print_backward(deque, echo);
    printf("}\nforward {\n");
    deque_print_forward(deque, echo);
    printf("}\n");
    size_t rt_value = 0;
    size_t size_val = 0;
    if (deque_pop_back(deque, &rt_value, sizeof(rt_value), &size_val)) {
        printf("%zu off back of deque\n", rt_value);
    } else {
        fprintf(stderr, "Error getting value from back of deque.\n");
        deque_free(deque);
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < 10; ++i) {
        if (!deque_push_back(deque, &i, sizeof(i))) {
            deque_free(deque);
            return EXIT_FAILURE;
        }
    }
    printf("forward {\n");
    deque_print_forward(deque, echo);
    printf("}\n");
    for (size_t i = 0; i < 10; ++i) {
        if (deque_pop_front(deque, &rt_value, sizeof(rt_value), &size_val)) {
            printf("Front: %zu\n", rt_value);
        } else {
            fprintf(stderr, "Error getting front of deque.\n");
            deque_free(deque);
            return EXIT_FAILURE;
        }
        printf("%zu items left in deque\n", deque_count(deque));
    }
    size_t peek_value = 0;
    if (deque_peek_front(deque, &peek_value, sizeof(peek_value))) {
        printf("peeked value: %zu\n", peek_value);
    } else {
        fprintf(stderr, "Error on peek of value: \n");
        deque_free(deque);
        return EXIT_FAILURE;
    }

    if (deque_peek_back(deque, &peek_value, sizeof(peek_value))) {
        printf("peek back: %zu\n", peek_value);
    } else {
        fprintf(stderr, "Error on peek.\n");
        deque_free(deque);
        return EXIT_FAILURE;
    }
    printf("%zu items left in deque\n", deque_count(deque));
    deque_free(deque);
    return EXIT_SUCCESS;
}
