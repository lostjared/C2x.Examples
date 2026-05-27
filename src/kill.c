#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<limits.h>

int main(int agc, char **argv) {
	 if(argc != 2) {
		 fprintf(stderr, "Error invalid arguments. Use:\n%s <pid>\n", argv[0]);
		 return EXIT_FAILURE;
	 }
	 char *ptr;
	 long value;
	 errno = 0;
	 value = strtol(argv[1],  &ptr, 10);

	 if(errno == ERANGE || ptr == nullptr || *ptr != '\0') {
		 fprintf(stderr, "Error on conversion of integer value.\n");
		 return EXIT_FAILURE;
	 }

	 if(value <= 0 || value >= INT_MAX) {
		 fprintf(stderr, "Error value out of range.\n");
		 return EXIT_FAILURE;
	 }

	 pid_t id = (pid_t)value

	return EXIT_SUCCESS;
}
