#include "count.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    static constexpr size_t arr_size = 10;
    srand((unsigned int)time(nullptr));

    int values[arr_size] = {0};
    int max = 0;

    for (size_t i = 0; i < arr_size; ++i) {
        values[i] = rand() % 0xFF;
        if (values[i] > max)
            max = values[i];
    }

    if (count_sort(values, arr_size, max + 1)) {
        for (size_t i = 0; i < arr_size; ++i) {
            printf("%d\n", values[i]);
        }
    } else {
        printf("Sort failed.\n");
    }

    return EXIT_SUCCESS;
}
