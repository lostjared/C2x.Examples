#include "encrypt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {

    if (argc != 5) {
        fprintf(stderr, "Error requires four arguments:\n%s<mode> <file> <output> <key>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int mode = 0;
    if(strcmp(argv[1], "-e") == 0) {
	    mode = 1;
    } else if(strcmp(argv[1], "-d") == 0) {
	    mode = 2;
    } else {
	    printf("Mode should be -e or -d for encypt/decypt.\n");
	    return EXIT_FAILURE;
    }

    FILE *fptr = fopen(argv[2], "rb");
    if (fptr == nullptr) {
        fprintf(stderr, "Error could not open file: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    FILE *optr = fopen(argv[3], "wb");
    if(optr == nullptr) {
	fprintf(stderr, "Error could not open file for output: %s\n", argv[3]);
	fclose(fptr);
	return EXIT_FAILURE;
    }
    fseek(fptr, 0, SEEK_END);
    auto s = ftell(fptr);
    rewind(fptr);
    if (s > 0) {
	size_t padded_size = ((s + 7) / 8) * 8;
	unsigned char buffer[8];
	unsigned char output[8];
	size_t bytes = 0;
        while(bytes = fread(buffer, sizeof(char), 8, fptr) > 0) {
		if(mode == 1) {
			des_encipher(buffer, output, (unsigned char *)argv[4]);
			fwrite(output, sizeof(unsigned char), bytes, fptr);
		} else {
			des_decipher(buffer, output, (unsigned char *)argv[4]);
			fwrite(output, sizeof(unsigned char), bytes, fptr);
		}
	}
        fclose(fptr);
	fclose(optr);
    } else {
	    fclose(fptr);
    }
    return EXIT_SUCCESS;
}
