#include "mq.h"
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error requires two arguments:%s <path> <prority>\n", argv[0]);
        return EXIT_FAILURE;
    }
    unsigned int prio = (unsigned int)atoi(argv[2]);
    mqd_t m;
    m = mq_open(argv[1], O_CREAT | O_WRONLY);
    if (m == (mqd_t)-1) {
        fprintf(stderr, "Error failed to create queue.\n");
        return EXIT_FAILURE;
    }
    struct mq_message msg;
    snprintf(msg.buffer, MQ_MESSAGE_SIZE, "Hello World.");
    msg.msg_type = 0;
    if (mq_send(m, (char *)&msg, sizeof(struct mq_message), prio) == -1) {
        fprintf(stderr, "Error sending message.\n");
        mq_close(m);
        return EXIT_FAILURE;
    }
    mq_close(m);
    return EXIT_SUCCESS;
}
