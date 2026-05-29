#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/wait.h>
#include<signal.h>
#include<unistd.h>
#include<time.h>


int main(void) {
	int fd[2];
	if(pipe(fd) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}
	signal(SIGPIPE, SIG_IGN);
	pid_t id;
	id = fork();
	if(id == -1) {
		perror("fork");
		return EXIT_FAILURE;
	} else if(id == 0) {
		srand((unsigned int)time(0));
		int message = rand()%255;
		printf("Sending message: %d\n", message);
		close(fd[0]);
		write(fd[1], &message, sizeof(message));
		close(fd[1]);
		_exit(EXIT_SUCCESS);
	} else {
		close(fd[1]);
		int message = 0;
		ssize_t rb = read(fd[0], &message, sizeof(message));
		if(rb > 0) {
			printf("Child returned: %d\n", message);
			close(fd[0]);
		}
	}	
	waitpid(id, 0, 0);
	return EXIT_SUCCESS;
}
