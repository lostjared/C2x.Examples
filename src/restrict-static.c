#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void only(size_t size, char buffer[restrict static size+1]) [[reproducible]] {
	for(size_t i = 0; i < size; ++i) {
		buffer[i] = 'X';
	}
}


int main(void) {
	char value[] = "test 1 2 3";
	only(strlen(value), value);
	printf("%s\n", value);
	return EXIT_SUCCESS;
}

