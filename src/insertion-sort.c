#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void insertion_sort(void *base, size_t count, size_t size,
                    int (*cmp)(const void *, const void *)) {
    unsigned char *arr = base;
    unsigned char *temp = malloc(size);
    if (temp == nullptr)
        return;

    for (size_t i = 1; i < count; ++i) {
        memcpy(temp, arr + i * size, size);
        size_t j = i;
        while (j > 0 && cmp(arr + (j - 1) * size, temp) > 0) {
            memcpy(arr + j * size, arr + (j - 1) * size, size);
            --j;
        }
        memcpy(arr + j * size, temp, size);
    }
    free(temp);
}

int compare_integer(const void *a, const void *b) {
    const int *x = a;
    const int *y = b;
    if (*x < *y)
        return -1;
    if (*x > *y)
        return 1;
    return 0;
}

int main(void) {
    size_t number = 0;
    size_t allocated = 1024 * 4;
    int *values = malloc(sizeof(int) * allocated);
    if (values == nullptr) {
        fprintf(stderr, "Error could not allocate memory.\n");
        return EXIT_FAILURE;
    }
    int val = 0;
    while ((fscanf(stdin, "%d", &val)) == 1) {
        if (number >= allocated) {
            allocated *= 2;
            int *temp = realloc(values, sizeof(int) * allocated);
            if (temp == nullptr) {
                fprintf(stderr, "Error on allocation of memory.\n");
                free(values);
                return EXIT_FAILURE;
            }
            values = temp;
        }
        values[number++] = val;
    }
    insertion_sort(values, number, sizeof(int), compare_integer);
    for (size_t i = 0; i < number; ++i)

        printf("%d\n", values[i]);
    free(values);
    return EXIT_SUCCESS;
}
