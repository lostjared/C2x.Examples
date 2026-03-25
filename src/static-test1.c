#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int grow(int value) [[unsequenced]] {
    return value + (value * 2);
}

void grow_int(size_t number, int amount[static number]) {
    for (size_t i = 0; i < number; i++) {
        amount[i] = grow(amount[i]);
    }
}

void print_int(size_t number, int amount[static number]) {
    for (size_t i = 0; i < number; ++i) {
        printf("%d ", amount[i]);
    }
    printf("\n");
}

int main(void) {
    static constexpr int arr_size = 5;
    int num[arr_size] = {0, 1, 2, 3, 4};
    print_int(arr_size, num);
    grow_int(arr_size, num);
    print_int(arr_size, num);
    return EXIT_SUCCESS;
}
