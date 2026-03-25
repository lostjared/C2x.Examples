#include "mx_socket.h"

bool mx_socket_listen(MXSocket *sock, const char *port, int backlog) {
    if (sock == nullptr || port == nullptr)
        return false;
    struct addrinfo hints;
    struct addrinfo *rt, *rp;
    int sfd, optval, s;

    memset(sock, 0, sizeof(MXSocket));
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

        close(sfd);
    }

    if (rp != nullptr) {
        if (listen(sfd, backlog) == -1) {
            freeaddrinfo(rt);
            return false;
        }
        sock->sockfd = sfd;
        sock->addrlen = rp->ai_addrlen;
    }
    freeaddrinfo(rt);
    return true;
}

bool mx_socket_accept(const MXSocket *input, MXSocket *output) {

    if (input == nullptr || output == nullptr)
        return false;

    int newfd = accept(input->sockfd, 0, 0);
    if (newfd == -1)
        return false;
    output->sockfd = newfd;
    output->addrlen = input->addrlen;
    output->blocking = input->blocking;
    printf("Socket: %d accepted...\n", newfd);
    return true;
}

void mx_socket_close(MXSocket *sock) {
    if (sock == nullptr)
        return;

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
        sock->blocking = state;
        if (fcntl(sock->sockfd, F_SETFL, flags) == -1) {
            fprintf(stderr, "Error setting flags for: %d\n", sock->sockfd);
            return false;
        }
    }
    return true;
}

bool mx_socket_connect(MXSocket *sock, const char *host, const char *port, int type) {
    if (sock == nullptr || host == nullptr || port == nullptr)
        return false;
    struct addrinfo hints;
    struct addrinfo *rt, *rp;
    int sfd, s;
    memset(&hints, 0, sizeof(struct addrinfo));
    memset(sock, 0, sizeof(MXSocket));
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

        close(sfd);
    }

    if (rp != nullptr) {
        sock->sockfd = sfd;
        sock->addrlen = rp->ai_addrlen;
    } else {
        freeaddrinfo(rt);
        return false;
    }

    freeaddrinfo(rt);
    return true;
}
