#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<threads.h>

struct data {
	sem_t s;
	int counter;
};

int thread_proc(void *ptr) {
	struct data *d = ptr;
	sem_wait(&d->s);
	sleep(1);
	d->counter ++;
	printf("Thread woke up: %d\n", d->counter);	
	sem_post(&d->s);
	return 0;
}


int main(void) {
	struct data d = {};
	if(sem_init(&d.s, 0, 1) == -1) {
		perror("sem_init");
		return EXIT_FAILURE;
	}
	thrd_t t;
	thrd_create(&t, thread_proc, &d);
	sem_wait(&d.s);
	d.counter ++;
	printf("Main thread woke up: %d...\n", d.counter);
	sem_post(&d.s);
	thrd_join(t, nullptr);
	sem_destroy(&d.s);
	return EXIT_SUCCESS;
}

