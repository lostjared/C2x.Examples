#include<stdio.h>
#include<stdlib.h>


size_t count_char(const char *restrict source, const size_t len, const char cmp, const size_t counter) [[reproducible]]  {
	size_t c = counter;
	for(size_t i = 0; i < len; ++i) {
		if(source[i] == cmp)  {
			++c;
		}
	}
	return c;
}

int main(int argc, char **argv) {
	if(argc<= 1) {
		fprintf(stderr, "Error requires at least one filename as argument.\n");
		return EXIT_FAILURE;
	}
	for(int i = 1; i < argc; ++i) {
		FILE *fptr = fopen(argv[i], "rb");
		if(fptr == nullptr) {
			fprintf(stderr, "Error could not open file: %s\n", argv[i]);
			continue;
		}
		static constexpr size_t BUFFER_SIZE = 4096;
		static char buffer[BUFFER_SIZE] = {};
		size_t bytes = 0;
		size_t counter = 0;
		while((bytes = fread(buffer, sizeof(char), BUFFER_SIZE, fptr)) != 0) {
			counter = count_char(buffer, bytes, '\n', counter);
		}
		printf("%s: %zu\n", argv[i], counter);
		fclose(fptr);
	}
	return EXIT_SUCCESS;
}


