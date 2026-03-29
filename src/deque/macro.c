#include "deque.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEQUEUE_CHECK(val)                                                           \
    do {                                                                             \
        if (!(val)) {                                                                \
            fprintf(stderr, "Error deque check failed %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                                      \
        }                                                                            \
    } while (0)

struct Event {
    size_t event_type;
    size_t index;
};

void echo(const void *ptr) {
    const struct Event *e = ptr;
    printf("The value is: %zu:%zu\n", e->event_type, e->index);
}

int main(void) {
    srand((unsigned int)time(nullptr));
    Deque *deque = nullptr;
    DEQUEUE_CHECK(deque_init(&deque, nullptr));
    for (size_t i = 0; i < 10; ++i) {
        struct Event e = {(size_t)rand() % 255, i};
        if ((i % 2) == 0) {
            DEQUEUE_CHECK(deque_push_front(deque, &e, sizeof(e)));
        } else {
            DEQUEUE_CHECK(deque_push_back(deque, &e, sizeof(e)));
        }
    }
    deque_print_forward(deque, echo);
    for (size_t i = 0; i < 10; ++i) {
        struct Event value;
        size_t rt_size = 0;
        DEQUEUE_CHECK(deque_pop_front(deque, &value, sizeof(value), &rt_size));
        printf("Value popped from front: %zu : %zu\n", value.event_type, value.index);
    }
    deque_free(deque);
    return EXIT_SUCCESS;
}
