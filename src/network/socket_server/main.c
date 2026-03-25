#include "../mx_socket.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *process_input(void *p) {
    MXSocket *sockfd = (MXSocket *)p;
    char buffer[4097];
    ssize_t bytes = 0;
    while ((bytes = mx_socket_read(sockfd, buffer, 4096, 0)) > 0) {
        buffer[bytes] = 0;
        printf("%s\n", buffer);
    }
    printf("Connection to Socket: %d closed\n", sockfd->sockfd);
    mx_socket_close(sockfd);
    free(sockfd);
    return 0;
}

void socket_listen(const char *port) {
    MXSocket sock;
    if (!mx_socket_init(&sock)) {
        fprintf(stderr, "Error init Socket struct\n");
        return;
    }
    bool active = true;
    if (mx_socket_listen(&sock, port, 5)) {
        printf("Listening on port %s\n", port);
        while (active) {
            MXSocket *new_socket = malloc(sizeof(MXSocket));
            if (new_socket == nullptr) {
                fprintf(stderr, "Memory exhausted.\n");
                mx_socket_close(&sock);
                return;
            }
            if (!mx_socket_init(new_socket)) {
                fprintf(stderr, "Error init socket struct.\n");
                mx_socket_close(&sock);
                free(new_socket);
                return;
            }
            if (mx_socket_accept(&sock, new_socket)) {
                pthread_t id;
                if (pthread_create(&id, nullptr, process_input, new_socket) == 0) {
                    pthread_detach(id);
                } else {
                    mx_socket_close(new_socket);
                    free(new_socket);
                    fprintf(stderr, "Error spawning new thread...\n");
                    active = false;
                    mx_socket_close(&sock);
                    return;
                }
            } else {
                free(new_socket);
            }
        }
    } else {
        fprintf(stderr, "Error on listen..\n");
        return;
    }
}

int main(int argc, char **argv) {

    if (argc == 2) {
        socket_listen(argv[1]);
    }
    return EXIT_SUCCESS;
}
