#ifndef ENCRYPT_H_
#define ENCRYPT_H_

typedef unsigned long Long;

typedef struct {
    Long e, n;
} RSA_PublicKey;

typedef struct {
    Long d, n;
} RSA_PrivateKey;

extern void des_encipher(const unsigned char *text, unsigned char *ciphertext, const unsigned char *key);
extern void des_decipher(const unsigned char *ciphertext, unsigned char *text, const unsigned char *key);
extern void rsa_encipher(Long plaintext, Long *ciphertext, RSA_PublicKey p_key);
extern void rsa_decipher(Long ciphertext, Long *plaintext, RSA_PrivateKey p_key);

#endif
