#include<stdio.h>
#include<stdlib.h>

static char const infile[] = {
#pragma Cedro 1.0 embed
#embed "embed.c"
};

int main(void) {
	auto bytes = sizeof(infile);
	printf("Size of embed %d\n", bytes);
	for(unsigned int i = 0; i < bytes; ++i) {
		fputc(infile[i], stdout);
	}
	fputc('\n', stdout);
	return EXIT_SUCCESS;
}
