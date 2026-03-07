#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void output_file(useconds_t duration, FILE *restrict fptr, FILE  *restrict output) {
	while(!feof(fptr)) {
		char c;
		fread(&c, 1, 1, fptr);
		fwrite(&c, 1, 1, output);
		fflush(output);
		usleep(duration);
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
	useconds_t t = (useconds_t) atoi(argv[2]);
	if(t > 0) {
		output_file(t, fptr, stdout);
	}
	fclose(fptr);
	return EXIT_SUCCESS;
}


