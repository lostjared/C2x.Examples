#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void output_file(useconds_t duration, FILE *restrict fptr, FILE  *restrict output) {
	size_t bytes = 0;
	static constexpr int buf_size = 4096;
	char c[buf_size];
	while((bytes = fread(c, sizeof(char), buf_size, fptr)) > 0) {	
		for(size_t i = 0; i < bytes; ++i) {
			fputc(c[i],output);
			fflush(output);
			usleep(duration);
		}
	}
	fflush(output);
}


int main(int argc, char *argv[argc+1]) {
	if(argc != 3) {
		fprintf(stderr, "Error invalid arguments use\n%s file timeout\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *fptr = fopen(argv[1], "rb");
	if(!fptr) {
		fprintf(stderr, "Could not open file %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	char *endptr;
	useconds_t t = (useconds_t)strtoul(argv[2], &endptr, 10);
	if(t > 0) {
		output_file(t, fptr, stdout);
	}
	fclose(fptr);
	return EXIT_SUCCESS;
}


