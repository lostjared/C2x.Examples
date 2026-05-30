#ifndef RWLIB_H_
#define RWLIB_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t write_all(int fd, const void *buf, size_t bytes);
ssize_t read_all(int fd, void *buf, size_t bytes);

#ifdef __cplusplus
}
#endif

#endif
