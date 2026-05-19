#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[argc+1]) {
	if(argc != 2) {
		fprintf(stderr, "Error incorrect arguments: %s <input>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int fd = open(argv[1], O_RDWR);
	if(fd == -1) {
		fprintf(stderr, "Error could not open file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	if(lseek(fd, -2, SEEK_END) == -1) {
		fprintf(stderr, "Error seeking to end.\n");
		close(fd);
		return EXIT_FAILURE;
	}
	char c = 0;
	ssize_t i = read(fd, &c, 1);
	if(i != 1) {
		fprintf(stderr, "Error reading from file\n");
		close(fd);
		return EXIT_FAILURE;
	}
	if(lseek(fd, -1, SEEK_CUR) == -1) {
		fprintf(stderr, "Error seeking.\n");
		close(fd);
		return EXIT_FAILURE;
	}
	c++;
	ssize_t w = write(fd, &c, 1);
	if(w != i) {
		fprintf(stderr, "Error writing to file\n");
		close(fd);
		return EXIT_FAILURE;
	}

	if(close(fd) == -1) {
		fprintf(stderr, "Error closing file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

