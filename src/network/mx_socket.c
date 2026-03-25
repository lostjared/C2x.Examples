#include "mx_socket.h"

bool mx_socket_listen(MXSocket *sock, const char *port, int backlog) {
    if (sock == nullptr || port == nullptr)
        return false;
    struct addrinfo hints;
    struct addrinfo *rt, *rp;
    int sfd = -1, optval, s;

    mx_socket_init(sock);
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    s = getaddrinfo(nullptr, port, &hints, &rt);
    if (s != 0)
        return false;

    optval = 1;

    for (rp = rt; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            close(sfd);
            freeaddrinfo(rt);
            return false;
        }

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

	if(sfd >= 0)
	    close(sfd);
	sfd = -1;
    }

    if(sfd == -1) {
	   freeaddrinfo(rt);
	   return false;
    }

    if (rp != nullptr && sfd >= 0) {
        if (listen(sfd, backlog) == -1) {
            freeaddrinfo(rt);
            close(sfd);
	    return false;
        }
        sock->sockfd = sfd;
        sock->addrlen = rp->ai_addrlen;
    } else { 
	if(sfd >= 0)
		close(sfd);
    	return false;
    }

    freeaddrinfo(rt);
    return true;
}

bool mx_socket_accept(const MXSocket *input, MXSocket *output) {
    if (input == nullptr || output == nullptr)
	return false;
 
    if(!mx_socket_valid(input))
	return false;

    int newfd = accept(input->sockfd, 0, 0);
    if (newfd == -1)
        return false;
    
    int flags = fcntl(newfd, F_GETFL);
    if(flags == -1) {
	    close(newfd);
	    return false;
    }

    if(input->blocking)
	    flags  &= ~O_NONBLOCK;
    else
	    flags |= O_NONBLOCK;

    if(fcntl(newfd, F_SETFL, flags) == -1) {
	    close(newfd);
	    return false;
    }

    if(mx_socket_valid(output))
	mx_socket_close(output);

    output->sockfd = newfd;
    output->addrlen = input->addrlen;
    output->blocking = input->blocking;
    printf("Accepted Socket: %d\n", newfd);
    return true;
}

void mx_socket_close(MXSocket *sock) {
    if (sock == nullptr)
        return;

    if(sock->sockfd >= 0)
	    close(sock->sockfd);
    sock->sockfd = -1;
}

bool mx_socket_set_blocking(MXSocket *sock, bool state) {
    if (sock == nullptr)
        return false;
    if (sock->sockfd >= 0) {
        int flags = fcntl(sock->sockfd, F_GETFL);
        if (flags == -1) {
            fprintf(stderr, "Error getting flags for: %d\n", sock->sockfd);
            return false;
        }
        if (state)
            flags &= ~O_NONBLOCK;
        else
            flags |= O_NONBLOCK;
        if (fcntl(sock->sockfd, F_SETFL, flags) == -1) {
            fprintf(stderr, "Error setting flags for: %d\n", sock->sockfd);
            return false;
        }
	sock->blocking = state;
    } else return false;
    return true;
}

bool mx_socket_connect(MXSocket *sock, const char *host, const char *port, int type) {
    if (sock == nullptr || host == nullptr || port == nullptr)
        return false;
    struct addrinfo hints;
    struct addrinfo *rt, *rp;
    int sfd = -1, s;
    memset(&hints, 0, sizeof(struct addrinfo));
    mx_socket_init(sock);
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    s = getaddrinfo(host, port, &hints, &rt);
    if (s != 0) {
        errno = ENOSYS;
        return false;
    }
    for (rp = rt; rp != nullptr; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        if(sfd >= 0) {
		close(sfd);
		sfd = -1;
	}
    }

    if (rp != nullptr) {
        sock->sockfd = sfd;
        sock->addrlen = rp->ai_addrlen;
    } else {
        freeaddrinfo(rt);
	if(sfd >= 0)
		close(sfd);
        return false;
    }

    freeaddrinfo(rt);
    return true;
}

void mx_socket_init(MXSocket *sock) {
	if(sock == nullptr)
		return false;
	memset(sock, 0, sizeof(MXSocket));
	sock->sockfd = -1;
}

bool mx_socket_valid(const MXSocket *sock) {
    if (sock == nullptr)
        return false;
    return sock->sockfd >= 0;
}

ssize_t mx_socket_read(MXSocket *sock, void *buf, size_t len, int flags) {
    if (sock == nullptr || buf == nullptr || len == 0)
        return -1;
    if (!mx_socket_valid(sock)) {
        errno = EBADF;
        return -1;
    }
    return recv(sock->sockfd, buf, len, flags);
}

ssize_t mx_socket_send(MXSocket *sock, const void *buf, size_t len, int flags) {
	if(sock == nullptr || buf == nullptr || len == 0)
		return -1;
	if(!mx_socket_valid(sock)) {
		errno = EBADF;
		return -1;
	}
	return send(sock->sockfd, buf, len, flags);
}

bool mx_socket_is_open(const MXSocket *sock) {
    if (sock == nullptr || !mx_socket_valid(sock))
        return false;
    char c = 0;
    ssize_t r = recv(sock->sockfd, &c, 1, MSG_PEEK | MSG_DONTWAIT);
    if (r == 0)
        return false;
    if (r > 0)
        return true;
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        return true;
    }
    return false;
}

bool mx_socket_readline(MXSocket *sock, char **buffer, size_t *size) {
    if (sock == nullptr || buffer == nullptr || size == nullptr || !mx_socket_valid(sock))
        return false;
    size_t init_size = 4096;
    char *temp = malloc(init_size + 1);
    if (temp == nullptr)
        return false;
    char c = 0;
    size_t index = 0;
    while (1) {
        ssize_t read_val = read(sock->sockfd, &c, 1);
        if (read_val == 0)
	       break;
       if(read_val == -1) {
	       free(temp);
	       return false;
       }
        if (c == '\n')
            break;
        if (index > init_size - 1) {
            init_size *= 2;
            char *t = realloc(temp, init_size + 1);
            if (t == nullptr) {
                free(temp);
                return false;
            }
	    temp = t;
        }
        temp[index++] = c;
    }
    temp[index] = 0;
    *buffer = temp;
    *size = index;
    return true;
}
