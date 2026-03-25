#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

mtx_t mut;

int thread_1(void *ptr) {
    const char *name = (const char *)ptr;
    for (int i = 0; i < 100; ++i) {
        if (mtx_lock(&mut) == thrd_success) {
            printf("Hello from %s, ; %d\n", name, i);
            mtx_unlock(&mut);
        }
    }
    return 0;
}

int main(void) {
    if (mtx_init(&mut, mtx_plain) != thrd_success) {
        fprintf(stderr, "Error initalizing mutex\n");
        return EXIT_FAILURE;
    }
    thrd_t id1, id2;
    thrd_create(&id1, thread_1, "Thread 1");
    thrd_create(&id2, thread_1, "Thread 2");
    thrd_join(id1, nullptr);
    thrd_join(id2, nullptr);
    mtx_destroy(&mut);
    return EXIT_SUCCESS;
}
