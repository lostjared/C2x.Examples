#include<stdio.h>
#include<stdlib.h>
#include "../mx_socket.h"

int main(int argc, char **argv) {
    if(argc != 4) {
        fprintf(stderr, "Error on invoke of dl:\nuse:\ndl <host> <port> <file>\n");
        return EXIT_FAILURE;
    }
    MXSocket socket;
    mx_socket_init(&socket);
    if(mx_socket_connect(&socket, argv[1], argv[2], SOCK_STREAM)) {
        printf("dl: connected.\n");
        static constexpr int buffer_size = 4096;
        char info[buffer_size];
        snprintf(info, buffer_size-1, "GET %s HTTP/1.0\n\n", argv[3]);
        ssize_t bytes = 0;
        if((bytes = mx_socket_send(&socket, info, strlen(info) + 1, MSG_NOSIGNAL)) > 0) {
            printf("dl: request sent.\n");
            char buffer[1024];
            ssize_t rbytes = 0;
            printf("data:\n");
            while((rbytes = mx_socket_read(&socket, buffer, 1024, 0)) > 0) {
                buffer[rbytes] = 0;
                printf("%s\n", buffer);
            }
        } else if(bytes == -1 && errno == EPIPE) {
            fprintf(stderr, "dl: Error on send, broken pipe.\n");
        }
        mx_socket_close(&socket);
    } else {
        fprintf(stderr, "Error on connect:\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
