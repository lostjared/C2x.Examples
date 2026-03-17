#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<threads.h>

int thread_1(void *) {
	for(int i = 0; i < 100; ++i) {
		printf("Hello World; %d\n", i);
	}
	return 0;
}

int main(void) {

	thrd_t id;
	thrd_create(&id, thread_1, nullptr);
	thrd_join(id, nullptr);

	return EXIT_SUCCESS;
}


