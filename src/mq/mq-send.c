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

    char *end = nullptr;
    unsigned int prio = (unsigned int)strtoul(argv[2], &end, 10);
    if(end == argv[2] || (end != nullptr && *end != '\0')) {
        fprintf(stderr, "Error invalid conversion of: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
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
