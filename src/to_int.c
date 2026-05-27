#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<errno.h>

bool to_int(const char *src, int *number) {
	long value = 0;
	char *p = nullptr;
	value = strtol(src, &p, 10);
	if(errno == ERANGE || p == src || *p != '\0') {
		fprintf(stderr, "Invalid integer input\n");
		*number = 0;
		return false;
	}
	if(value <= INT_MIN || value >= INT_MAX) {
		fprintf(stderr, "Error invalid integer range.\n");
		*number = 0;
		return false;
	}
	*number = (int)value;
	return true;
}


int main(int argc, char **argv) {
	if(argc != 2) {
		fprintf(stderr, "Error argument count should be exactly one argument.\n%s <number>\n", argv[0]);
		return EXIT_FAILURE;
	}
	int value = 0;
	if(to_int(argv[1], &value)) {
		printf("Conversion value: %d\n", value);
		return EXIT_SUCCESS;
	} else {
		printf("Conversion failed.\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


