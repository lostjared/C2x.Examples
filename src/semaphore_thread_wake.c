#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>

struct data {
    sem_t s;
    int counter;
};

int thread_proc(void *ptr) {
    struct data *d = ptr;
    sem_wait(&d->s);
    printf("Thread %d  woke up.\n", d->counter);
    sleep(1);
    return 0;
}

int main(void) {
    struct data d = {};
    if (sem_init(&d.s, 0, 1) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    thrd_t t;
    thrd_create(&t, thread_proc, &d);
    d.counter++;
    printf("Waking thread: %d...\n", d.counter);
    sem_post(&d.s);
    thrd_join(t, nullptr);
    printf("Thread joined.\n");
    sem_destroy(&d.s);
    return EXIT_SUCCESS;
}
