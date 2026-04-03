#include "quicksort.h"
#include <stdlib.h>
#include <string.h>

void insertion_sort(void *base, size_t count, size_t size, int (*cmp)(const void *, const void *)) {
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

static int partition(void *data, size_t esize, int i, int k, int (*compare)(const void *, const void *)) {

    if(data == nullptr || esize == 0 || compare == nullptr)
        return -1;

    char *arr = data;
    void *p_val, *temp;

    p_val = malloc(esize);
    if (p_val == nullptr)
        return -1;
    temp = malloc(esize);
    if (temp == nullptr) {
        free(p_val);
        return -1;
    }

    size_t z = (size_t)(rand() % (k - i + 1)) + (size_t)i;
    memcpy(p_val, &arr[z * esize], esize);
    i--;
    k++;

    while (1) {
        do {
            k--;
        } while (compare(&arr[(size_t)k * esize], p_val) > 0);
        do {
            i++;
        } while (compare(&arr[(size_t)i * esize], p_val) < 0);

        if (i >= k) {
            break;
        } else {
            memcpy(temp, &arr[(size_t)i * esize], esize);
            memcpy(&arr[(size_t)i * esize], &arr[(size_t)k * esize], esize);
            memcpy(&arr[(size_t)k * esize], temp, esize);
        }
    }
    free(p_val);
    free(temp);
    return k;
}

bool quick_sort(void *data, size_t size, size_t esize, int i, int k, int (*compare)(const void *, const void *)) {
    if(data == nullptr || size == 0 || esize == 0 || compare == nullptr || i > k)
        return false;
    int j = 0;
    while (i < k) {
        if ((j = partition(data, esize, i, k, compare)) < 0)
            return false;
        if (quick_sort(data, size, esize, i, j, compare) == false)
            return false;
        i = j + 1;
    }
    return true;
}
