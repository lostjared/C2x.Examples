
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "heap.h"

int compare_max(const void *a, const void *b) {
    const size_t *ia = a;
    const size_t *ib = b;
    if(*ia > *ib)
        return 1;
    else if(*ia < *ib)
        return -1;
    return 0;
}

int compare_min(const void *a, const void *b) {
    const size_t *ia = a;
    const size_t *ib = b;
    if(*ia < *ib)
        return 1;
    else if(*ia > *ib)
        return -1;
    return 0;
}


int main(void) {

    static constexpr size_t count = 10;
    size_t values[10] =  { 0, 5, 1, 2, 4 ,6, 7, 8, 15, 25};
    //max heap
    if(!heap_sort_ex(values, count, sizeof(size_t), compare_max)) {
        fprintf(stderr, "Error sort failed.\n");
        return EXIT_FAILURE;
    }
    printf("Max:\n");
    for(size_t i = 0; i < 10; ++i) {
        printf("%zu\n", values[i]);
    }

    if(!heap_sort_ex(values, count, sizeof(size_t), compare_min)) {
        fprintf(stderr, "Error sort failed.\n");
        return EXIT_FAILURE;
    }

    printf("Min:\n");
    for(size_t i = 0; i < 10; ++i) {
        printf("%zu\n", values[i]);
    }
    return 0;
}