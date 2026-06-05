#ifndef _MQ_H_
#define _MQ_H_

static constexpr int MQ_MESSAGE_SIZE = 64;

struct mq_message {
    char buffer[MQ_MESSAGE_SIZE];
    int msg_type;
};

#endif
