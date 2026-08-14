#include <errno.h>
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
static volatile sig_atomic_t active = 1;

void sig_handler(int sig [[maybe_unused]]) {
    active = 0;
}

int main(void) {
    int fd = -1;
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    if (mkfifo(FIFO_PATH, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        unlink(FIFO_PATH);
        return EXIT_FAILURE;
    }
    while (active == 1) {
        ssize_t len = 0;
        char buffer[MESSAGE_LEN] = {0};
        while ((len = read(fd, buffer, MESSAGE_LEN - 1)) > 0) {
            buffer[strcspn(buffer, "\r\n")] = '\0';
            printf("Message: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) {
                printf("Quit Message Sent..\n");
                active = 0;
                break;
            }
            memset(buffer, 0, sizeof(buffer));
        }
        if (len == 0) {
            close(fd);
            fd = open(FIFO_PATH, O_RDONLY);
            if (fd == -1)
                break;
        }
    }
    if (fd != -1)
        close(fd);
    unlink(FIFO_PATH);
    return EXIT_SUCCESS;
}
