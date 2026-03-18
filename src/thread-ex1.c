#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<threads.h>

mtx_t mut;

int thread_1(void *) {
	for(int i = 0; i < 100; ++i) {
		mtx_lock(&mut);
		printf("Hello from thread1, ; %d\n", i);
		mtx_unlock(&mut);
	}
	return 0;
}

int thread_2(void *) {
	for(int i = 0; i < 100; ++i) {
		mtx_lock(&mut);
		printf("Hello form thread2, : %d\n", i);
		mtx_unlock(&mut);
	}
	return 0;
}

int main(void) {
 	mtx_init(&mut, mtx_plain);
	thrd_t id1, id2;
	thrd_create(&id1, thread_1, nullptr);
	thrd_create(&id2, thread_2, nullptr);
	thrd_join(id1, nullptr);
	thrd_join(id2, nullptr);
	mtx_destroy(&mut);
	return EXIT_SUCCESS;
}


