#include<signal.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void signal_handler(int sig [[maybe_unused]]) {
	puts("\nSignal SIGINT caught.");
	exit(EXIT_SUCCESS);
}

int main(void) {

	if(signal(SIGINT, signal_handler) == SIG_ERR) {
		fprintf(stderr, "Error initalizing signal.\n");
		return EXIT_FAILURE;
	}

	int index = 0;
	while(1) {
		++index;
		sleep(3);
		printf("Count index: %d\n", index);
	}
	return 0;
}



