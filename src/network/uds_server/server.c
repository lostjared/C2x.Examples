#include "../mx_socket.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static constexpr size_t BUFFER_SIZE = 1024;

void proc_cmd(int clientfd) {
    ssize_t bytes = 0;
    char buffer[BUFFER_SIZE] = {};
    while ((bytes = read(clientfd, buffer, BUFFER_SIZE)) > 0)
        if (write(STDOUT_FILENO, buffer, (size_t)bytes) != bytes)
            perror("write");
    if (bytes == -1)
        perror("read");
}

sig_atomic_t close_socket = 0;

void close_signal(int sig [[maybe_unused]]) {
    close_socket = 1;
}

int main(void) {
    MXSocket sock;
    int clientfd = -1;
    struct sigaction sa = {};
    sa.sa_handler = close_signal;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    if (mx_socket_unix_listen(&sock, "/tmp/un_socket1.sock", 5)) {
        while (1) {
            if (close_socket == 1) {
                break;
            }
            clientfd = accept(sock.sockfd, 0, 0);
            if (clientfd == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("accept");
                continue;
            }
            proc_cmd(clientfd);
            if (close(clientfd) == -1)
                perror("close");
        }
    }
    printf("\nExiting...\n");
    mx_socket_close(&sock);
    return EXIT_SUCCESS;
}
