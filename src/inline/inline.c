
#include "inline.h"
#include <stdio.h>
#include <stdlib.h>

inline void init() {
    for_each(0, 10, "Hello, World");
    for_loop(0, 10, "Goodbye, World");
}

extern inline void print(const char[static 1]);

int main() {
    init();
    print("Hello, World inline");
    return 0;
}
