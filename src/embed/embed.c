#include <stdio.h>
#include <stdlib.h>

static char const infile[] = {
#embed "embed.c"
};

int main(void) {
    auto bytes = sizeof(infile);
    printf("Size of embed %ld\n", bytes);
    for (unsigned long i = 0; i < bytes; ++i) {
        fputc(infile[i], stdout);
    }
    fputc('\n', stdout);
    return EXIT_SUCCESS;
}
