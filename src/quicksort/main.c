#include "quicksort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void *a, const void *b) {
    const size_t *ia = a;
    const size_t *ib = b;
    if (*ia > *ib)
        return 1;
    else if (*ia < *ib)
        return -1;
    return 0;
}

int main(void) {
    srand((unsigned int)time(nullptr) ^ (unsigned int)rand() % 0xFFFFFF);
    static constexpr size_t arr_size = 10;
    size_t values[arr_size] = {};
    for (size_t i = 0; i < arr_size; ++i) {
        values[i] = (size_t)rand() % arr_size;
    }
    if (!quick_sort(values, arr_size, sizeof(size_t), 0, arr_size - 1, compare)) {
        fprintf(stderr, "Error sorting..\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < arr_size; ++i) {
        printf("%zu\n", values[i]);
    }
    return EXIT_SUCCESS;
}
