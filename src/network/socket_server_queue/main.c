#include "../mx_socket.h"
#include "dequeue.h"
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_cond_t cond;
MXSocket g_listen_socket;
pthread_mutex_t mut;
Dequeue *queue;
atomic_bool server_running = true;

typedef struct {
    char *buffer;
    size_t type;
} QueueData;

char *_strdup(const char *str) {
    size_t bytes = strlen(str) + 1;
    char *temp = malloc(bytes);
    if (temp == nullptr)
        return nullptr;
    memcpy(temp, str, bytes);
    return temp;
}

void destroy(void *p) {
    QueueData *q = p;
    if (q != nullptr) {
        if (q->buffer != nullptr) {
            free(q->buffer);
            q->buffer = nullptr;
        }
    }
}

void *process_input(void *p) {
    MXSocket *sockfd = (MXSocket *)p;
    char buffer[4097];
    ssize_t bytes = 0;
    while ((bytes = mx_socket_read(sockfd, buffer, 4096, 0)) > 0) {
        buffer[bytes] = 0;
        if (pthread_mutex_lock(&mut) == 0) {
            QueueData d;
            memset(&d, 0, sizeof(QueueData));
            d.buffer = _strdup(buffer);

            if (d.buffer == nullptr) {
                pthread_mutex_unlock(&mut);
                break;
            }
            if (strcmp(d.buffer, "exit") == 0)
                d.type = 2;
            else
                d.type = 1;
            size_t cmd_type = d.type;
            if (!dequeue_push_back(queue, &d, sizeof(QueueData))) {
                fprintf(stderr, "Error pushing into queue.\n");
                free(d.buffer);
                if (pthread_mutex_unlock(&mut) != 0) {
                    fprintf(stderr, "Error unlocking mutex.\n");
                }
                break;
            }
            pthread_cond_signal(&cond);
            printf("Pushed: %s\n", d.buffer);
            if (pthread_mutex_unlock(&mut) != 0) {
                fprintf(stderr, "Error could not unlock mutex.\n");
                break;
            }
            if (cmd_type == 2) {
                atomic_store(&server_running, false);
                mx_socket_close(&g_listen_socket);
                pthread_cond_broadcast(&cond);
                break;
            }
        }
    }
    printf("Connection to Socket: %d closed\n", sockfd->sockfd);
    mx_socket_close(sockfd);
    free(sockfd);
    return 0;
}

void *proc_queue(void *) {
    if (pthread_mutex_lock(&mut) != 0)
        return 0;

    while (atomic_load(&server_running)) {
        while (atomic_load(&server_running) && dequeue_count(queue) == 0) {
            if (pthread_cond_wait(&cond, &mut) != 0) {
                fprintf(stderr, "Error waiting on condition variable.\n");
                if (pthread_mutex_unlock(&mut) != 0) {
                    fprintf(stderr, "Error on unlock.\n");
                }
                return 0;
            }
        }
        if (!atomic_load(&server_running) && dequeue_count(queue) == 0)
            break;

        QueueData d;
        memset(&d, 0, sizeof(QueueData));
        size_t bytes = 0;
        if (!dequeue_pop_front(queue, &d, sizeof(QueueData), &bytes)) {
            fprintf(stderr, "failed to pop front.\n");
            if (pthread_mutex_unlock(&mut) != 0) {
                fprintf(stderr, "Error on unlock.\n");
            }
            break;
        }
        pthread_mutex_unlock(&mut);
        printf("New Data Message: %s of Type: %zu\n", d.buffer, d.type);
        if (d.type == 2) {
            printf("Shutting down..\n");
            free(d.buffer);
            break;
        }
        free(d.buffer);
        if (pthread_mutex_lock(&mut) != 0) {
            fprintf(stderr, "Error locking mutex.\n");
            break;
        }
    }
    if (pthread_mutex_unlock(&mut) != 0) {
        fprintf(stderr, "Error on unlock.\n");
    }
    return 0;
}

bool socket_listen(const char *port) {
    MXSocket sock;
    if (!mx_socket_init(&sock)) {
        fprintf(stderr, "Error init Socket struct\n");
        return false;
    }
    if (pthread_mutex_init(&mut, nullptr) != 0) {
        fprintf(stderr, "Error on mutex init\n");
        return false;
    }
    if (pthread_cond_init(&cond, nullptr) != 0) {
        fprintf(stderr, "Error on condition variable init\n");
        pthread_mutex_destroy(&mut);
        return false;
    }
    if (!dequeue_init(&queue, nullptr)) {
        fprintf(stderr, "Error on queue init\n");
        pthread_mutex_destroy(&mut);
        pthread_cond_destroy(&cond);
        return false;
    }
    if (!mx_socket_listen(&sock, port, 5)) {
        fprintf(stderr, "Error on listen..\n");
        dequeue_free(queue);
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mut);
        return false;
    }
    g_listen_socket = sock;
    if (!mx_socket_set_blocking(&sock, false)) {
        fprintf(stderr, "Error setting listening socket nonblocking.\n");
        mx_socket_close(&sock);
        dequeue_free(queue);
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mut);
        return false;
    }
    printf("Listening on port %s\n", port);
    pthread_t qt;
    if (pthread_create(&qt, nullptr, proc_queue, nullptr) != 0) {
        fprintf(stderr, "Error on creation of queue thread.\n");
        mx_socket_close(&sock);
        dequeue_free(queue);
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mut);
        return false;
    }
    while (atomic_load(&server_running)) {
        struct pollfd pfd;
        pfd.fd = sock.sockfd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        int rc = poll(&pfd, 1, 250);
        if (!atomic_load(&server_running))
            break;
        if (rc < 0) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "poll failed: %s\n", strerror(errno));
            break;
        }
        if (rc == 0)
            continue;
        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            break;
        }

        if (pfd.revents & POLLIN) {
            while (1) {
                MXSocket *new_socket = malloc(sizeof(MXSocket));
                if (new_socket == nullptr) {
                    fprintf(stderr, "Memory exhausted.\n");
                    atomic_store(&server_running, false);
                    break;
                }
                if (!mx_socket_init(new_socket)) {
                    fprintf(stderr, "Error init socket struct.\n");
                    free(new_socket);
                    atomic_store(&server_running, false);
                    break;
                }
                if (!mx_socket_accept(&sock, new_socket)) {
                    free(new_socket);
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                    if (!atomic_load(&server_running))
                        break;
                    fprintf(stderr, "accept failed: %s\n", strerror(errno));
                    atomic_store(&server_running, false);
                    break;
                }
                if (!mx_socket_set_blocking(new_socket, true)) {
                    fprintf(stderr, "Error setting accepted socket blocking.\n");
                    mx_socket_close(new_socket);
                    free(new_socket);
                    continue;
                }
                pthread_t id;
                if (pthread_create(&id, nullptr, process_input, new_socket) == 0) {
                    pthread_detach(id);
                } else {
                    fprintf(stderr, "Error spawning new thread...\n");
                    mx_socket_close(new_socket);
                    free(new_socket);
                    atomic_store(&server_running, false);
                    break;
                }
            }
        }
    }
    atomic_store(&server_running, false);
    mx_socket_close(&sock);
    pthread_cond_broadcast(&cond);
    pthread_join(qt, nullptr);
    dequeue_free(queue);
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
    return true;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        if (socket_listen(argv[1])) {
            printf("Exited.\n");
        }
    }
    return EXIT_SUCCESS;
}
