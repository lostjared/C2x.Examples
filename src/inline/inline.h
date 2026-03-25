#ifndef __inline_h_
#define __inline_h_

#include <stdio.h>

static inline void for_each(int start, int stop, const char *src) {
    for (int i = start; i < stop; ++i) {
        printf("%d: %s\n", i, src);
    }
}

#define for_loop(start, stop, hello)   \
    for (int i = start; i < stop; ++i) \
        printf("%d: %s\n", i, hello);

extern inline void init();

inline void print(const char buffer[static 1]) {
    for (int i = 0; i < 3; ++i) {
        puts(buffer);
    }
}

#endif
