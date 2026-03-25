#include <stdio.h>
#include <stdlib.h>

unsigned char code[] = {
#if defined(__has_embed) && __has_embed("if-embed.c")
#embed "if-embed.c"
#else
#error "embed directive is missing"
#endif
};

void echo_text(const unsigned char *const text, size_t s, FILE *fptr) {
    for (size_t i = 0; i < s && text[i] != 0; ++i) {
        fputc(text[i], fptr);
    }
}

int main(void) {
    echo_text(code, sizeof(code), stdout);
    return EXIT_SUCCESS;
}
