#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/FIFO_TEST"
#define MESSAGE "Hello, World"

int main(void) {
    int fd;
    if (mkfifo(FIFO_NAME, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    if (write(fd, MESSAGE, strlen(MESSAGE)) != strlen(MESSAGE)) {
        perror("write");
    }
    close(fd);
    return EXIT_SUCCESS;
}
