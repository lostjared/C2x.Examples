#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

ssize_t write_all(int fd, const void *buf, size_t bytes) {
    const char *ptr = buf;
    size_t left = bytes;
    while (left > 0) {
        ssize_t written = write(fd, ptr, left);
        if (written == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        left -= (size_t)written;
        ptr += written;
    }
    return (ssize_t)bytes;
}

ssize_t read_all(int fd, void *buf, size_t bytes) {
    char *ptr = buf;
    size_t left = bytes;
    while (left > 0) {
        ssize_t bytes_read = read(fd, ptr, left);
        if (bytes_read == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (bytes_read == 0)
            break;
        left -= (size_t)bytes_read;
        ptr += bytes_read;
    }
    return (ssize_t)(bytes - left);
}

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    signal(SIGPIPE, SIG_IGN);
    pid_t id;
    id = fork();
    if (id == -1) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (id == 0) {
        srand((unsigned int)time(0));
        int message = rand() % 255;
        printf("Sending message: %d\n", message);
        close(fd[0]);
        write_all(fd[1], &message, sizeof(message));
        close(fd[1]);
        _exit(EXIT_SUCCESS);
    } else {
        close(fd[1]);
        int message = 0;
        ssize_t rb = read_all(fd[0], &message, sizeof(message));
        if (rb > 0) {
            printf("Child returned: %d\n", message);
            close(fd[0]);
        }
    }
    waitpid(id, 0, 0);
    return EXIT_SUCCESS;
}
