#ifndef MX_SOCKET_H
#define MX_SOCKET_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
typedef struct {
    int sockfd;
    socklen_t addrlen;
    bool blocking;
} MXSocket;

bool mx_socket_init(MXSocket *s);
bool mx_socket_listen(MXSocket *s, const char *port, int backlog);
bool mx_socket_accept(const MXSocket *input, MXSocket *output);
void mx_socket_close(MXSocket *s);
bool mx_socket_set_blocking(MXSocket *s, bool state);
bool mx_socket_connect(MXSocket *s, const char *host, const char *port, int type);
ssize_t mx_socket_read(MXSocket *s, void *data, size_t len, int flags);
ssize_t mx_socket_send(MXSocket *s, const void *data, size_t len, int flags);
bool mx_socket_valid(const MXSocket *s);
bool mx_socket_is_open(const MXSocket *s);
bool mx_socket_readline(MXSocket *s, char **buffer, size_t *len);

#endif
