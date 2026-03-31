#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void *a, const void *b) {
    const size_t *ia = a;
    const size_t *ib = b;
    if (*ia < *ib)
        return -1;
    else if (*ia > *ib)
        return 1;
    return 0;
}

int main(void) {
    srand((unsigned int)time(nullptr));
    Heap heap;
    size_t values[10];
    if (!heap_init(&heap, compare, nullptr)) {
        fprintf(stderr, "Could not init heap.\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < 10; ++i) {
        values[i] = (size_t)rand() % 255;
        if (!heap_insert(&heap, &values[i])) {
            fprintf(stderr, "Error inserting into heap.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
    }
    void *value = 0;
    for (size_t i = 0; i < 10; ++i) {
        if (!heap_extract(&heap, &value)) {
            fprintf(stderr, "Error extracting from heap..\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        printf("Top of heap: %zu\n", (*(size_t *)value));
    }
    heap_destroy(&heap);
    return EXIT_SUCCESS;
}
