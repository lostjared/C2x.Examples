#ifndef MX_SOCKET_H
#define MX_SOCKET_H

#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<fcntl.h>


typedef struct {
	int sockfd;
        socklen_t addrlen;
        bool blocking;
} MXSocket;

bool mx_socket_listen(MXSocket *s, const char *port, int backlog);
bool mx_socket_accept(const MXSocket *input, MXSocket *output); 
void mx_socket_close(MXSocket *s);
bool mx_socket_set_blocking(MXSocket *s, bool state);
bool mx_socket_connect(MXSocket *s, const char *host, const char *port, int type);

#endif
