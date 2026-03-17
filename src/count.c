#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h>

size_t compare(const char c, const size_t counter) [[unsequenced]]  {
	if(c == '\n')
		return counter+1;
	return counter;
}

size_t count(const char *restrict src, const size_t len) [[reproducible]]  {
        size_t counter = 0;
	for(size_t i  = 0; i < len; ++i) {
		counter = compare(src[i], counter);
	}
	return counter;
}

int main(int argc, char **argv) {
	if(argc != 2) {
		fprintf(stderr, "Error incorrect args use:\ncount <file>\n");
		return EXIT_FAILURE;
	}
	size_t counter = 0;
	FILE *fptr = fopen(argv[1], "rb");
	if(fptr == nullptr) {
		fprintf(stderr, "Error could not open file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	size_t bytes = 0;
	static constexpr size_t BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE] = {};
	while((bytes = fread(buffer, sizeof(char), BUFFER_SIZE, fptr)) != 0) {
		counter += count(buffer, bytes);
	}
	fclose(fptr);
	printf("%s: %zu\n", argv[1], counter);
	return EXIT_SUCCESS;
}


