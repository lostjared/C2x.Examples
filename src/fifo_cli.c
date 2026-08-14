#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/fifo_file_X"
static constexpr size_t MESSAGE_LEN = 256;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error invalid arguments take one argument.\n");
        exit(EXIT_FAILURE);
    }
    int fd = open(FIFO_PATH, O_WRONLY);
    if (!fd) {
        perror("open");
        return EXIT_FAILURE;
    }
    size_t len = strlen(argv[1]);
    if (len > MESSAGE_LEN - 1) {
        fprintf(stderr, "Message too Long.\n");
        return EXIT_FAILURE;
    }
    if (write(fd, argv[1], len) != (ssize_t)len) {
        perror("write");
    }
    close(fd);
    return EXIT_SUCCESS;
}
