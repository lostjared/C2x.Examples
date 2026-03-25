#include <stdio.h>
#include <stdlib.h>

void testArg(int argc, const char *args[static argc + 1]) {
    for (int i = 0; i < argc; ++i) {
        printf("argument (%d): %s\n", i, args[i]);
    }
}

int main(void) {
    const char *names[] = {"jared", "dude", "guy", "man"};
    testArg(4, names);
    const char *buffer[] = {"yo", "sup", "howdy", "hello"};
    testArg(10, buffer);
    return EXIT_SUCCESS;
}
