#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define result_ok(value, it)                                                              \
    if (value <= 0) {                                                                     \
        printf("iteration: %d result < 0: %d %s %s\n", it, __LINE__, __FILE__, __func__); \
        exit(EXIT_FAILURE);                                                               \
    }

int random_number() {
    return rand() % 5;
}

int main() {
    srand((unsigned int)time(nullptr));
    for (int i = 0; i < 255; ++i) {
        result_ok(rand() % 5, i);
    }
    return EXIT_SUCCESS;
}
