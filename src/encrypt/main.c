#include "encrypt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
static uint64_t fnv1a64_bytes(const void *data, size_t n, uint64_t h) [[unsequenced]] {
    const uint8_t *p = (const uint8_t *)data;
    for (size_t i = 0; i < n; ++i) {
        h ^= p[i];
        h *= 1099511628211ull;
    }
    return h;
}

static uint64_t fnv1a64_file(const char filepath[static 1]) {
    FILE *f = fopen(filepath, "rb");
    if (!f)
        return 0;
    uint64_t h = 1469598103934665603ull;
    static constexpr size_t BUF_SIZE = 16384;
    char buf[BUF_SIZE];
    size_t bytes = 0;

    while ((bytes = fread(buf, sizeof(char), BUF_SIZE, f)) != 0) {
        h = fnv1a64_bytes(buf, bytes, h);
    }
    fclose(f);
    return h;
}

int main(int argc, char **argv) {

    if (argc != 5) {
        fprintf(stderr, "Error requires four arguments:\n%s<mode> <file> <output> <key>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int mode = 0;
    if (strcmp(argv[1], "-e") == 0) {
        mode = 1;
    } else if (strcmp(argv[1], "-d") == 0) {
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
    if (optr == nullptr) {
        fprintf(stderr, "Error could not open file for output: %s\n", argv[3]);
        fclose(fptr);
        return EXIT_FAILURE;
    }
    unsigned char buffer[8] = {};
    unsigned char output[8] = {};
    size_t bytes = 0;
    size_t total = 0;
    if (mode == 1) {
        while ((bytes = fread(buffer, 1, 8, fptr)) > 0) {
            if (bytes < 8) {
                unsigned char pad_val = 8 - (unsigned char)bytes;
                memset(buffer + bytes, pad_val, pad_val);
           }
            des_encipher(buffer, output, (unsigned char *)argv[4]);
            fwrite(output, 1, 8, optr);
            total += 8;
        }
        if (ftell(fptr) % 8 == 0) {
            memset(buffer, 8, 8);
            des_encipher(buffer, output, (unsigned char *)argv[4]);
            fwrite(output, 1, 8, optr);
            total += 8;
        }
    } else {
        fseek(fptr, 0, SEEK_END);
        auto file_size = ftell(fptr);
        rewind(fptr);
        while ((bytes = fread(buffer, 1, 8, fptr)) == 8) {
            des_decipher(buffer, output, (unsigned char *)argv[4]);
            if (ftell(fptr) == file_size) {
                unsigned char pad_val = output[7];
                if (pad_val > 0 && pad_val <= 8) {
                    fwrite(output, 1, 8 - pad_val, optr);
                    total += (8 - pad_val);
                    break;
                }
            }
            fwrite(output, 1, 8, optr);
            total += 8;
        }
    }
    fclose(fptr);
    fclose(optr);
    if (mode == 1) {
	printf("Original File Hash: %lx\n", fnv1a64_file(argv[2]));
    } else {
	printf("Decrypted File Hash: %lx\n", fnv1a64_file(argv[3]));
    }
    return EXIT_SUCCESS;
}
