#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void handler(int) {
	printf("SIGINT .\n");
	exit(EXIT_SUCCESS);
}

int main(void) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGINT, &sa, NULL) == -1) {
	 fprintf(stderr, "Error on sigaction.\n");
 	 return EXIT_FAILURE;
    }
    size_t inc = 0;
    while(1) {
	    printf("Ctrl+C -Stop Loop Index: %zu\n", inc);
	    sleep(1);
	    ++inc;
    }
    return EXIT_SUCCESS;
}

