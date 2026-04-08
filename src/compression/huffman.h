#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    unsigned char symbol;
    int freq;
} HuffNode;

typedef struct {
    unsigned char used;
    uint32_t code;
    unsigned char size;
} HuffCode;

int huffman_compress(const unsigned char *original, unsigned char **compressed, int size);
int huffman_decompress(const unsigned char *compressed, unsigned char **original);

#endif
