#include "mq.h"
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

sig_atomic_t quit_loop = 0;

void quit_signal(int) {
    quit_loop = 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error requires one argument: message queue\n");
        return EXIT_FAILURE;
    }
    mqd_t m;
    struct mq_attr cattr = {};
    cattr.mq_maxmsg = 10;
    cattr.mq_msgsize = sizeof(struct mq_message);
    m = mq_open(argv[1], O_CREAT | O_RDONLY, 0644, &cattr);
    if (m == (mqd_t)-1) {
        perror("mq_open");
        return EXIT_FAILURE;
    }
    struct sigaction sa = {};
    sa.sa_handler = quit_signal;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        perror("sigaction");
        mq_close(m);
        return EXIT_FAILURE;
    }

    struct mq_attr attr;
    if (mq_getattr(m, &attr) == -1) {
        fprintf(stderr, "Error getting attributes.\n");
        mq_close(m);
        return EXIT_FAILURE;
    }
    while (1) {
        if (quit_loop == 1)
            break;
        struct mq_message msg;
        memset(&msg, 0, sizeof(struct mq_message));
        unsigned int prio = 0;
        ssize_t bytes = 0;
        bytes = mq_receive(m, (char *)&msg, sizeof(struct mq_message), &prio);
        if (bytes == -1) {
            if (errno == EINTR)
                continue;
            perror("mq_receive");
            mq_close(m);
            return EXIT_FAILURE;
        }
        printf("%s\n", msg.buffer);
    }
    printf("\nExiting..\n");
    mq_close(m);
    return EXIT_SUCCESS;
}
