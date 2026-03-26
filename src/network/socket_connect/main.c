#include "../mx_socket.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *f_getline(char *buffer, size_t bytes, FILE *fptr) {
    if (buffer == nullptr || bytes < 2 || fptr == nullptr)
        return nullptr;

    char *ret = fgets(buffer, (int)bytes, fptr);
    if (ret == nullptr)
        return nullptr;

    char *position = strchr(buffer, '\n');
    if (position != nullptr) {
        *position = 0;
    } else {
        int ch = 0;
        while ((ch = fgetc(fptr)) != '\n' && ch != EOF) {
        }
    }
    return ret;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "%s: <ip> <port>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    MXSocket sock;
    if (!mx_socket_init(&sock)) {
        fprintf(stderr, "Error initializing socket.\n");
        return EXIT_FAILURE;
    }
    if (mx_socket_connect(&sock, argv[1], argv[2], SOCK_STREAM)) {
        printf("Connected to: %s:%s\n", argv[1], argv[2]);
        printf("Type exit to shutdown\n");
        char buffer[4096];
        bool active = true;
        while (active) {
            printf("> ");
            if (f_getline(buffer, 4096, stdin) == nullptr) {
                continue;
            }
            if (strcmp(buffer, "quit") == 0) {
                printf("Quit message sent shutting down..\n");
                mx_socket_close(&sock);
                return EXIT_SUCCESS;
            }
            ssize_t bytes = 0;
            if ((bytes = mx_socket_send(&sock, buffer, strlen(buffer) + 1, MSG_NOSIGNAL)) > 0) {
                printf("Data sent... %zd bytes\n", bytes);
            } else if (bytes == -1 && errno == EPIPE) {
                printf("Connected closed: broken pipe..\n");
                return EXIT_SUCCESS;
            }
        }
    } else {
        fprintf(stderr, "Error on connect  to %s:%s\n", argv[1], argv[2]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
