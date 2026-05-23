#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void) {
	printf("process id: %d\n", getpid());
	return EXIT_SUCCESS;
}

