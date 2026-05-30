#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include"rwlib/rw.h"

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
        srand((unsigned int)time(nullptr));
        int message = rand() % 255;
        printf("Sending message: %d\n", message);
        close(fd[0]);
        if (write_all(fd[1], &message, sizeof(message)) == -1) {
            perror("write");
        }
        close(fd[1]);
        _exit(EXIT_SUCCESS);
    } else {
        close(fd[1]);
        int message = 0;
        ssize_t rb = read_all(fd[0], &message, sizeof(message));
        if (rb == (ssize_t)sizeof(message)) {
            printf("Child returned: %d\n", message);
        } else if (rb == -1)
            perror("read");
        else
            fprintf(stderr, "Unexpected EOF (end of file) or partial read\n");
        close(fd[0]);
    }
    waitpid(id, 0, 0);
    return EXIT_SUCCESS;
}
