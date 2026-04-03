#include "mergesort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void *a, const void *b) {
    const size_t *ia = a;
    const size_t *ib = b;
    if (*ia > *ib)
        return -1;
    else if (*ia < *ib)
        return 1;
    return 0;
}

int main(void) {
    static constexpr size_t arr_size = 10;
    size_t values[arr_size] = {0};
    srand((unsigned int)time(nullptr));
    for (size_t i = 0; i < arr_size; ++i) {
        values[i] = (size_t)rand() % 0xFF;
    }
    if (!merge_sort_ex(values, sizeof(values[0]), 0, arr_size - 1, compare)) {
        fprintf(stderr, "Error on sort.\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < arr_size; ++i) {
        printf("%zu\n", values[i]);
    }
    return EXIT_SUCCESS;
}
