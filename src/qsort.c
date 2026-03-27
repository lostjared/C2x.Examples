#include <stdio.h>
#include <stdlib.h>

int compare_value(const void *a, const void *b) {
    const int *i_a = a;
    const int *i_b = b;
    if(*i_a > *i_b)
	    return 1;
    else if(*i_a < *i_b)
	    return -1;
    return 0;
}

int main() {
    static constexpr int ARR_SIZE = 6;
    int array[ARR_SIZE] = {
        [0] = 1,
        [1] = 5,
        [2] = 3,
        [3] = 0,
        [4] = -1,
        [5] = 7};
    qsort(&array[0], ARR_SIZE - 1, sizeof(int), compare_value);
    for (unsigned int i = 0; i < ARR_SIZE; ++i) {
        printf("%d: %d\n", i, array[i]);
    }
    return 0;
}
