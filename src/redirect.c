#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[argc + 1]) {
    if (argc != 2) {
        fprintf(stderr, "Error argument not provied. Use:\n%s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    freopen(argv[1], "w", stdout);
    printf("Redirected..\n");
    fclose(stdout);
    return EXIT_SUCCESS;
}
