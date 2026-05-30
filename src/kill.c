#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[argc + 1]) {
    if (argc != 2) {
        fprintf(stderr, "Error invalid arguments. Use:\n%s <pid>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *ptr;
    long value;
    errno = 0;
    value = strtol(argv[1], &ptr, 10);
    if (errno == ERANGE || ptr == argv[1] || *ptr != '\0') {
        fprintf(stderr, "Error on conversion of integer value.\n");
        return EXIT_FAILURE;
    }
    if (value <= 0 || value >= INT_MAX) {
        fprintf(stderr, "Error value out of range.\n");
        return EXIT_FAILURE;
    }
    pid_t id = (pid_t)value;
    if (kill(id, SIGINT) == -1) {
        fprintf(stderr, "Error on kill: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
