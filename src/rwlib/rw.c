#include "rw.h"
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

ssize_t write_all(int fd, const void *buf, size_t bytes) {
    const char *ptr = buf;
    size_t left = bytes;
    while (left > 0) {
        ssize_t written = write(fd, ptr, left);
        if (written == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        left -= (size_t)written;
        ptr += written;
    }
    return (ssize_t)bytes;
}

ssize_t read_all(int fd, void *buf, size_t bytes) {
    char *ptr = buf;
    size_t left = bytes;
    while (left > 0) {
        ssize_t bytes_read = read(fd, ptr, left);
        if (bytes_read == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (bytes_read == 0)
            break;
        left -= (size_t)bytes_read;
        ptr += bytes_read;
    }
    return (ssize_t)(bytes - left);
}
