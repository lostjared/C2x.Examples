#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<sys/wait.h>

struct data {
	sem_t s;
	int counter;
};

int main(void) {
	constexpr int iter = 3;

	struct data *dat = mmap(nullptr, sizeof(struct data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(dat == MAP_FAILED) {
		perror("mmap");
		return EXIT_FAILURE;
	}
	dat->counter = 0;

	if(sem_init(&dat->s, 1, 1) != 0) {
		perror("sem_init");
		munmap(dat, sizeof(struct data));
		return EXIT_FAILURE;
	}

	pid_t id = fork();
	if(id < 0) {
		perror("fork");
		return EXIT_FAILURE;
	}
	
	if(id == 0) {
		for(int i = 0; i < iter; ++i) {
			sem_wait(&dat->s);
			dat->counter++;
			printf("Child -> incremented counter to %d\n", dat->counter);
			sem_post(&dat->s);
			sleep(1);
		}
		return EXIT_SUCCESS;
	} else {
		for(int i = 0; i < iter; ++i) {
			sem_wait(&dat->s);
			dat->counter++;
			printf("Parent -> incremented counter to %d\n", dat->counter);
			sem_post(&dat->s);
			sleep(1);
		}
		wait(nullptr);
		printf("Final Counter value: %d expected: %d\n", dat->counter, iter * 2);
		sem_destroy(&dat->s);
		munmap(dat, sizeof(struct data));
	}
	return EXIT_SUCCESS;
}

