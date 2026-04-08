#include "encrypt.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

static int bit_get(const unsigned char *bits, int pos) {
    unsigned char mask = 0x80;
    for (int i = 0; i < (pos % 8); ++i) {
        mask = mask >> 1;
    }
    return (((mask & bits[(size_t)(pos / 8)]) == mask) ? 1 : 0);
}

static void bit_set(unsigned char *bits, int pos, int state) {
    unsigned char mask = 0x80;
    for (int i = 0; i < (pos % 8); ++i) {
        mask = mask >> 1;
    }

    if (state) {
        bits[pos / 8] = bits[pos / 8] | mask;
    } else {
        bits[pos / 8] = bits[pos / 8] & (~mask);
    }
}

void bit_xor(const unsigned char *bits1, const unsigned char *bits2, unsigned char *bitsx, int size) {
    for (int i = 0; i < size; i++) {
        if (bit_get(bits1, i) != bit_get(bits2, i)) {
            bit_set(bitsx, i, 1);
        } else {
            bit_set(bitsx, i, 0);
        }
    }
}

void bit_rot_left(unsigned char *bits, int size, int count) {
    int fbit, lbit, i, j;

    if (size > 0) {
        for (j = 0; j < count; j++) {
            for (i = 0; i <= ((size - 1) / 8); i++) {
                lbit = bit_get(&bits[i], 0);
                if (i == 0) {
                    fbit = lbit;
                } else {
                    bit_set(&bits[i - 1], 7, lbit);
                }
                bits[i] = bits[i] << 1;
            }
            bit_set(bits, size - 1, fbit);
        }
    }
}

static const int DES_Transform[56] = {
    57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};

static const int DES_Rotations[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

static const int DES_Permuted[48] = {
    14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

static const int DES_Initial[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7};

static const int DES_Expansion[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};

static const int DES_Sbox[8][4][16] = {
    {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

static const int DES_Pbox[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25};

static const int DES_Final[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25};

static void permute(unsigned char *bits, const int *mapping, int n) {
    unsigned char temp[8];
    size_t bytes = (n + 7) / 8;

    memset(temp, 0, bytes);

    for (int i = 0; i < n; ++i) {
        bit_set(temp, i, bit_get(bits, mapping[i] - 1));
    }

    memcpy(bits, temp, bytes);
}

typedef enum DEnc {
    DIR_ENC,
    DIR_DEC
} Direction;

static int DES_main(const unsigned char *source, unsigned char *target, const unsigned char *key, Direction direction) {
    static unsigned char subkeys[16][7];
    unsigned char temp[8];
    unsigned char lkey[4], rkey[4];

    if (key != nullptr) {
        memcpy(temp, key, 8);
        permute(temp, DES_Transform, 56);
        memset(lkey, 0, 4);
        memset(rkey, 0, 4);

        for (int j = 0; j < 28; ++j) {
            bit_set(lkey, j, bit_get(temp, j));
        }
        for (int j = 0; j < 28; ++j) {
            bit_set(rkey, j, bit_get(temp, j + 28));
        }

        for (int i = 0; i < 16; ++i) {
            bit_rot_left(lkey, 28, DES_Rotations[i]);
            bit_rot_left(rkey, 28, DES_Rotations[i]);

            for (int j = 0; j < 28; ++j)
                bit_set(subkeys[i], j, bit_get(lkey, j));

            for (int j = 0; j < 28; ++j)
                bit_set(subkeys[i], j + 28, bit_get(rkey, j));

            permute(subkeys[i], DES_Permuted, 48);
        }
    }
    memcpy(temp, source, 8);
    permute(temp, DES_Initial, 64);
    unsigned char lblk[6];
    unsigned char rblk[6];

    memcpy(lblk, &temp[0], 4);
    memcpy(rblk, &temp[4], 4);

    unsigned char fblk[6];
    unsigned char xblk[6];

    unsigned char sblk = 0;

    for (int i = 0; i < 16; ++i) {
        memcpy(fblk, rblk, 4);
        permute(fblk, DES_Expansion, 48);
        if (direction == DIR_ENC) {
            bit_xor(fblk, subkeys[i], xblk, 48);
            memcpy(fblk, xblk, 6);
        } else {
            bit_xor(fblk, subkeys[15 - i], xblk, 48);
            memcpy(fblk, xblk, 6);
        }
        int p = 0;
        for (int j = 0; j < 8; ++j) {
            int row = (bit_get(fblk, (j * 6) + 0) * 2) + (bit_get(fblk, (j * 6) + 5) * 1);
            int col = (bit_get(fblk, (j * 6) + 1) * 8) + (bit_get(fblk, (j * 6) + 2) * 4) +
                      (bit_get(fblk, (j * 6) + 3) * 2) + (bit_get(fblk, (j * 6) + 4) * 1);

            sblk = (unsigned char)DES_Sbox[j][row][col];
            for (int k = 4; k < 8; ++k) {
                bit_set(fblk, p, bit_get(&sblk, k));
                ++p;
            }
        }
        permute(fblk, DES_Pbox, 32);
        bit_xor(lblk, fblk, xblk, 32);
        memcpy(lblk, rblk, 4);
        memcpy(rblk, xblk, 4);
    }
    memcpy(&target[0], rblk, 4);
    memcpy(&target[4], lblk, 4);

    permute(target, DES_Final, 64);
    return 0;
}

void des_encipher(const unsigned char *text, unsigned char *ciphertext, const unsigned char *key) {
    DES_main(text, ciphertext, key, DIR_ENC);
}

void des_decipher(const unsigned char *ciphertext, unsigned char *text, const unsigned char *key) {
    DES_main(ciphertext, text, key, DIR_DEC);
}

void rsa_encipher(Long plaintext, Long *ciphertext, RSA_PublicKey p_key) {
}

void rsa_decipher(Long ciphertext, Long *plaintext, RSA_PrivateKey p_key) {
}
