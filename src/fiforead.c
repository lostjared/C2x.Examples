#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>

#define FIFO_NAME "/tmp/FIFO_TEST"

int main(void) {
	int fd;
	fd = open(FIFO_NAME, O_RDONLY);
	if(fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}
	char buffer[255];
	ssize_t len = 0;
	if((len = read(fd, buffer, sizeof(buffer))) > 0) {
		buffer[len] = '\0';
		printf("%s\n", buffer);
	}
	close(fd);
	return EXIT_SUCCESS;
}


