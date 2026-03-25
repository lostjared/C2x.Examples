#include "ll.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void echo(const void *ptr) {
    printf("List value: %zu\n", *((size_t *)ptr));
}

int main(void) {

    LList *lst;
    list_init(&lst);
    for (size_t i = 0; i < 100; ++i) {
        list_insert(lst, &i, sizeof(i));
    }
    list_print(lst, echo);
    list_free(lst);
    return EXIT_SUCCESS;
}
