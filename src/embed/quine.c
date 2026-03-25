#include <stdio.h>
#include <stdlib.h>

const unsigned char code[] = {
#embed "quine.c"
};

int main(void) {
    fwrite(code, 1, sizeof(code), stdout);
    return EXIT_SUCCESS;
}
