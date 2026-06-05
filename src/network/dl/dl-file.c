#include "../mx_socket.h"
#include <stdio.h>
#include <stdlib.h>

static constexpr size_t BUFFER_SIZE = 4096;

struct http_header {
    char *header, *body;
    size_t body_length;
};

bool extract_header(MXSocket *socket, struct http_header *h) {
    size_t buffer_size_value = BUFFER_SIZE;
    size_t length = 0;
    char *buffer = (char *)malloc(BUFFER_SIZE + 1);
    ssize_t rbytes = 0;
    char data[1024];
    data[0] = 0;
    while (1) {
        rbytes = mx_socket_read(socket, data, sizeof(data), 0);
        if (rbytes <= 0) {
            fprintf(stderr, "Connection closed: Error.\n");
            free(buffer);
            return false;
        }

        if (length + (size_t)rbytes >= buffer_size_value) {
            buffer_size_value *= 2;
            char *n_buffer = realloc(buffer, buffer_size_value + 1);
            if (n_buffer == nullptr) {
                perror("realloc");
                free(buffer);
                return false;
            }
            buffer = n_buffer;
        }
        memcpy(buffer + length, data, (size_t)rbytes);
        length += (size_t)rbytes;
        buffer[length] = '\0';
        auto loc = strstr(buffer, "\r\n\r\n");
        if (loc != nullptr) {
            ssize_t pos = loc - buffer;
            h->header = (char *)malloc((size_t)pos + 1);
            if (h->header == nullptr) {
                free(buffer);
                perror("malloc");
                return false;
            }
            memcpy(h->header, buffer, (size_t)pos);
            h->header[pos] = '\0';
            size_t body_start = (size_t)pos + 4;
            h->body_length = length - body_start;
            if (h->body_length > 0) {
                h->body = (char *)malloc(h->body_length);
                if (h->body == nullptr) {
                    perror("malloc");
                    free(h->header);
                    free(buffer);
                    return false;
                }
                memcpy(h->body, buffer + body_start, h->body_length);
            } else {
                h->body = nullptr;
            }
            free(buffer);
            return true;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Error on invoke of dl-file:\nuse:\ndl <host> <port> <file> <output>\n");
        return EXIT_FAILURE;
    }
    MXSocket socket;
    mx_socket_init(&socket);
    if (mx_socket_connect(&socket, argv[1], argv[2], SOCK_STREAM)) {
        printf("dl: connected.\n");
        static constexpr int buffer_size = 4096;
        char info[buffer_size];
        snprintf(info, buffer_size - 1, "GET %s HTTP/1.0\r\nHOST: %s\r\n\r\n", argv[3], argv[1]);
        ssize_t bytes = 0;
        if ((bytes = mx_socket_send(&socket, info, strlen(info) + 1, MSG_NOSIGNAL)) > 0) {
            printf("dl: request sent.\n");
            struct http_header h;
            if (extract_header(&socket, &h)) {
                printf("Header: %s\n", h.header);
                free(h.header);
                free(h.body);
            }

        } else if (bytes == -1 && errno == EPIPE) {
            fprintf(stderr, "dl: Error on send, broken pipe.\n");
        }
        mx_socket_close(&socket);
    } else {
        fprintf(stderr, "Error on connect:\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
