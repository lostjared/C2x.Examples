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
	void *addr;
	size_t shm_size;

	fd = shm_open("/proc_program", O_RDONLY, 0);
	if(fd == -1) {
		perror("shm_open");
		return EXIT_FAILURE;
	}

	struct stat st_buf;
	if(fstat(fd, &st_buf) == -1) {
		perror("fstat");
		return EXIT_FAILURE;
	}
	
	shm_size = (size_t)st_buf.st_size;

	addr = mmap(nullptr, shm_size, PROT_READ, MAP_SHARED, fd, 0);
	if(addr == MAP_FAILED) {
		perror("mmap");
		return EXIT_FAILURE;
	}

	close(fd);
	struct shared_data *d = (struct shared_data *)addr;
	printf("%d\n", d->value);

	if(munmap(addr, shm_size) == -1) {
		perror("munmap");
	}

	if(shm_unlink("/proc_program") == -1) {
		perror("shm_unlink");
	}

	return EXIT_SUCCESS;
}

