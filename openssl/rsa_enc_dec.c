#include "rsa_enc_dec.h"
#include <stdio.h>

int padding = RSA_PKCS1_PADDING;

//create rsa struct
RSA *create_bio_rsa(unsigned char *key, int public)
{
    RSA *rsa = NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio == NULL) {
        printf("Failed to create key BIO");
        return 0;
    }
    if (public) {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    } else {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }
    if (rsa == NULL) {
        printf("Failed to create RSA");
    }
    BIO_free(keybio);

    return rsa;
}

RSA *create_rsa(FILE* fp, int public)
{
    RSA *rsa = NULL;
    if (public) {
        rsa = PEM_read_RSA_PUBKEY(fp, &rsa, NULL, NULL);
    } else {
        rsa = PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL);
    }
    if (rsa == NULL) {
        printf("Failed to create RSA");
    }

    return rsa;
}

int public_bio_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted)
{
    RSA *rsa = create_bio_rsa(key, 1);
    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
    RSA_free(rsa);
    return result;
}
int public_encrypt(unsigned char *data, int data_len, FILE* fp, unsigned char *encrypted)
{
    RSA *rsa = create_rsa(fp, 1);
    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
    RSA_free(rsa);
    return result;
}

int private_bio_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted)
{
    RSA *rsa = create_bio_rsa(key, 0);
    int  result = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

int private_decrypt(unsigned char *enc_data, int data_len, FILE* fp, unsigned char *decrypted)
{
    RSA *rsa = create_rsa(fp, 0);
    int  result = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

int private_bio_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted)
{
    RSA *rsa = create_bio_rsa(key, 0);
    int result = RSA_private_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}
int private_encrypt(unsigned char *data, int data_len, FILE *fp, unsigned char *encrypted)
{
    RSA *rsa = create_rsa(fp, 0);
    int result = RSA_private_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}
int public_bio_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted)
{
    RSA *rsa = create_bio_rsa(key, 1);
    int  result = RSA_public_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

int public_decrypt(unsigned char *enc_data, int data_len, FILE* fp, unsigned char *decrypted)
{
    RSA *rsa = create_rsa(fp, 1);
    int  result = RSA_public_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

void printLastError(char *msg)
{
    char err[130];;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n", msg, err);
}

//extract public key from private key
void extract_pub_from_private(unsigned char *key,FILE* fp)
{
    RSA *rsa = NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio == NULL) {
        printf("Failed to create key BIO");
        exit(1);
    }
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    if (rsa == NULL) {
        printf("Failed to create RSA");
        exit(1);
    }
    if(!PEM_write_RSA_PUBKEY(fp,rsa)){
        printf("Failed to write public key to file");
        exit(1);
    }

    RSA_free(rsa);
    BIO_free(keybio);
}

