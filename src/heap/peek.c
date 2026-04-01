#include "heap.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void *a, const void *b) {
    const size_t *ia = a, *ib = b;
    if (*ia > *ib)
        return 1;
    else if (*ia < *ib)
        return -1;
    return 0;
}

int main(void) {
    printf("peek example:\n");
    srand((unsigned int)time(nullptr));
    Heap heap;
    if (!heap_init(&heap, compare, nullptr)) {
        fprintf(stderr, "Error on init of heap.\n");
        return EXIT_FAILURE;
    }
    size_t values[500];
    for (size_t i = 0; i < 500; ++i) {
        values[i] = (size_t)rand() % 1000;
        if (!heap_insert(&heap, &values[i])) {
            fprintf(stderr, "Error on insert..\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        printf("inserted: %zu\n", values[i]);
    }

    printf("heap size: %zu contains {\n", heap.size);
    while (heap.size > 0) {
        void *tmp = nullptr;
        if (heap_peek(&heap, &tmp)) {
            size_t *value = tmp;
            if (*value <= 10) {
                break;
            }
            printf("\t%zu\n", *value);
        } else {
            fprintf(stderr, "Peek failed.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        if (!heap_extract(&heap, &tmp)) {
            fprintf(stderr, "Error on extract.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
    }
    printf("}\n");
    if (heap.size > 0) {
        printf("%zu numbers 10 or less left in heap {:\n", heap.size);
        while (heap.size > 0) {
            void *tmp = nullptr;
            if (!heap_extract(&heap, &tmp)) {
                fprintf(stderr, "Error on extract.\n");
                heap_destroy(&heap);
                return EXIT_FAILURE;
            }
            size_t *s = tmp;
            printf("\t%zu\n", *s);
        }
        printf("}\n");
    }
    heap_destroy(&heap);
    return EXIT_SUCCESS;
}