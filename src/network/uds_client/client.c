#include "../../rwlib/rw.h"
#include "../mx_socket.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    MXSocket sock;
    if (mx_socket_unix_connect(&sock, "/tmp/un_socket1.sock")) {
        size_t sent = 0;
        const char *buffer = "Hello, World UDS.\n";
        size_t length = strlen(buffer);
        if ((sent = (size_t)write_all(sock.sockfd, buffer, length)) == length) {
            printf("Sent %zu bytes.\n", sent);
        } else {
            printf("Send failed only wrote: %zu bytes\n", sent);
        }
        mx_socket_close(&sock);
    } else {
        fprintf(stderr, "Error on connect.\n");
        return false;
    }
    return EXIT_SUCCESS;
}
