#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc [[maybe_unused]], char **argv) {
    auto len = strlen("Test 1 2 3");
    printf("len is: %ld bytes\n", sizeof(len));
    auto const x = argv[0];
    printf("argv[0] = %s\n", x);
    auto value = strtoull("0x1234", nullptr, 16);
    printf("%lld : %ld bytes\n", value, sizeof(value));
    return EXIT_SUCCESS;
}
