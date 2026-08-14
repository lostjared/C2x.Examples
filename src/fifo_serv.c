#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<errno.h>
#include<string.h>

#define FIFO_PATH "/tmp/fifo_file_X"
static constexpr size_t MESSAGE_LEN = 256;


int main(void) {
	int fd = -1;
	if(mkfifo(FIFO_PATH, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
                perror("mkfifo");
                return EXIT_FAILURE;
        }
        fd = open(FIFO_PATH, O_RDONLY);
	if(!fd) {
		perror("open");
		return EXIT_FAILURE;
	}
	bool active = true;
	while(active) {
		ssize_t len = 0;		
		char buffer[MESSAGE_LEN] = {0};
		while((len = read(fd, buffer, MESSAGE_LEN)) > 0) {
			printf("Message: %s\n", buffer);
			if(strcmp(buffer, "exit") == 0) {
				printf("Quit Message Sent..\n");
				active = false;
				break;
			}
		}
	}
	close(fd);
	return EXIT_SUCCESS;
}

