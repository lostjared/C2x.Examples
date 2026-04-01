#include "heap.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int compare(const void *a, const void *b) {
    const size_t *ia = a, *ib = b;
    if (*ia < *ib)
        return 1;
    else if (*ia < *ib)
        return -1;
    return 0;
}

int main(void) {
    static constexpr size_t arr_size = 8;
    size_t values[arr_size] = {0, 5, 4, 3, 0, 7, 8, 25};
    if (!heap_sort_t(values, arr_size, sizeof(size_t), compare)) {
        fprintf(stderr, "Error on sort.\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < arr_size; ++i) {
        printf("%zu\n", values[i]);
    }
    return EXIT_SUCCESS;
}
