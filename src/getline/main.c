#include "getline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char buffer[1024];
    printf("Enter a value: ");
    if (f_getline(buffer, 1023, stdin)) {
        printf("User inputted: %s\n", buffer);
    } else {
        puts("Nothing inputted");
    }
    return EXIT_SUCCESS;
}
