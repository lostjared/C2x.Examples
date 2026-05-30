#include "rwlib/rw.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

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
        close(fd[0]);
        for (int i = 0; i < 250; ++i) {
            int message = 1 + rand() % 255;
            printf("Sending message to parent: %d\n", message);
            if (write_all(fd[1], &message, sizeof(message)) == -1) {
                perror("write");
            }
        }
        int exit_message = 0;
        if (write_all(fd[1], &exit_message, sizeof(exit_message)) == -1) {
            perror("write");
        }
        close(fd[1]);
        _exit(EXIT_SUCCESS);
    } else {
        close(fd[1]);
        while (1) {
            int message = 0;
            ssize_t rb = read_all(fd[0], &message, sizeof(message));
            if (rb == (ssize_t)sizeof(message)) {
                printf("Child returned: %d\n", message);
                if (message == 0) {
                    printf("Exit message\n");
                    break;
                }
            }
        }
        close(fd[0]);
    }
    waitpid(id, 0, 0);
    return EXIT_SUCCESS;
}
