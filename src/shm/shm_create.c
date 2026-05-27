#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include"shared.h"



int main() {

	int fd;

	fd = shm_open("/proc_program", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if(fd == -1) {
		perror("shm_open");
		return EXIT_FAILURE;
	}

	printf("Truncating.. \n");
	if(ftruncate(fd, sizeof(struct shared_data)) == -1) {
		perror("ftruncate..\n");
		return EXIT_FAILURE;
	}

	void *addr;
	addr = mmap(nullptr, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(addr == MAP_FAILED) {
		perror("mmap");
		return EXIT_FAILURE;
	}

	struct shared_data *d;
	d = (struct shared_data *)addr;
	d->value = 1;
	printf("Set value to 1\n");

	return EXIT_SUCCESS;
}

