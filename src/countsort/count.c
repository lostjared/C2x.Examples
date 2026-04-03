#include "count.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool count_sort(int *data, int size, int k) {
    if (data == nullptr || size == 0 || k == 0)
        return false;
    int *count, *temp;
    count = malloc((size_t)k * sizeof(int));
    if (count == nullptr)
        return false;
    temp = malloc((size_t)size * sizeof(int));
    if (temp == nullptr) {
        free(count);
        return false;
    }
    for (int i = 0; i < k; ++i)
        count[i] = 0;
    for (int z = 0; z < size; ++z)
        count[data[z]] = count[data[z]] + 1;
    for (int i = 1; i < k; ++i)
        count[i] = count[i] + count[i - 1];
    for (int z = size - 1; z >= 0; z--) {
        temp[count[data[z]] - 1] = data[z];
        count[data[z]] = count[data[z]] - 1;
    }
    memcpy(data, temp, (size_t)size * sizeof(int));
    free(count);
    free(temp);
    return true;
}
