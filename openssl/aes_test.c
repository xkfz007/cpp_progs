/*
 * aes.cc
 * - Show the usage of AES encryption/decryption
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>

int main1(int argc, char **argv)
{
    AES_KEY aes;
    unsigned char key[AES_BLOCK_SIZE];        // AES_BLOCK_SIZE = 16ES_BLOCK_SIZE
    unsigned char iv[AES_BLOCK_SIZE];        // init vector
    unsigned char *input_string;
    unsigned char *encrypted_string;
    unsigned char *decrypted_string;
    unsigned int len;        // encrypt length (in multiple of AES_BLOCK_SIZE)
    unsigned int i;

    // check usage
    if (argc != 2) {
        fprintf(stderr, "%s <plain text>\n", argv[0]);
        exit(-1);
    }

    // set the encryption length
    len = 0;
    if ((strlen(argv[1]) + 1) % AES_BLOCK_SIZE == 0) {
        len = strlen(argv[1]) + 1;
    } else {
        len = ((strlen(argv[1]) + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }

    // set the input string
    input_string = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (input_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for input_string\n");
        exit(-1);
    }
    strncpy((char *)input_string, argv[1], strlen(argv[1]));

    // Generate AES 128-bit key
    for (i = 0; i < 16; ++i) {
        key[i] = 32 + i;
    }

    // Set encryption key
    for (i = 0; i < AES_BLOCK_SIZE; ++i) {
        iv[i] = 0;
    }
    if (AES_set_encrypt_key(key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }

    // alloc encrypted_string
    encrypted_string = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (encrypted_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for encrypted_string\n");
        exit(-1);
    }

    // encrypt (iv will change)
    AES_cbc_encrypt(input_string, encrypted_string, len, &aes, iv, AES_ENCRYPT);

    // alloc decrypted_string
    decrypted_string = (unsigned char *)calloc(len, sizeof(unsigned char));
    if (decrypted_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for decrypted_string\n");
        exit(-1);
    }

    // Set decryption key
    for (i = 0; i < AES_BLOCK_SIZE; ++i) {
        iv[i] = 0;
    }
    if (AES_set_decrypt_key(key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set decryption key in AES\n");
        exit(-1);
    }

    // decrypt
    AES_cbc_encrypt(encrypted_string, decrypted_string, len, &aes, iv,
                    AES_DECRYPT);

    // print
    printf("input_string = %s\n", input_string);
    printf("encrypted string = ");
    for (i = 0; i < len; ++i) {
        printf("%x%x", (encrypted_string[i] >> 4) & 0xf,
               encrypted_string[i] & 0xf);
    }
    printf("\n");
    printf("decrypted string = %s\n", decrypted_string);

    return 0;
}

int aes_encrypt(unsigned char key[16], unsigned char iv[16],
                unsigned char *input_string,
                unsigned char *encrypted_string)
{

    AES_KEY aes;
    int len;

    // set the encryption length
    len = 0;
    if ((strlen(input_string) + 1) % 16 == 0) {
        len = strlen(input_string) + 1;
    } else {
        len = ((strlen(input_string) + 1) / 16 + 1) * 16;
    }

    if (AES_set_encrypt_key(key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }

    // encrypt (iv will change)
    AES_cbc_encrypt(input_string, encrypted_string, len, &aes, iv, AES_ENCRYPT);

    return len;
}

void aes_decrypt(unsigned char key[16], unsigned char iv[16],
                 unsigned char *encrypted_string,
                 unsigned char *decrypted_string,
                 int len)
{
    AES_KEY aes;

    if (len % 16 != 0) {
        printf("Encryt length is not multiple of 16\n");
        exit(1);
    }

    if (AES_set_decrypt_key(key, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set decryption key in AES\n");
        exit(-1);
    }
    // decrypt
    AES_cbc_encrypt(encrypted_string, decrypted_string, len, &aes, iv,
                    AES_DECRYPT);
}

int main(int argc, char **argv)
{

    int i;
    unsigned char input_string[] = "Look, it is a plane";
    unsigned char key[16];        // AES_BLOCK_SIZE = 16ES_BLOCK_SIZE
    unsigned char iv[16];        // init vector
    int encrypted_length;
    unsigned char encrypted_string[1024];
    unsigned char decrypted_string[1024];
    // Generate AES 128-bit key
    for (i = 0; i < 16; ++i) {
        key[i] = 32 + i;
    }

    // Set encryption key
    for (i = 0; i < 16; ++i) {
        iv[i] = 0;
    }
    encrypted_length = aes_encrypt(key, iv, input_string, encrypted_string);

    printf("input_string = %s\n", input_string);
    printf("encrypted string = ");
    for (i = 0; i < encrypted_length; ++i) {
        printf("%x%x", (encrypted_string[i] >> 4) & 0xf,
               encrypted_string[i] & 0xf);
    }
    printf("\n");
    for (i = 0; i < encrypted_length; ++i) {
        printf("%c", encrypted_string[i]);
    }
    printf("\n");

    // Set encryption key
    for (i = 0; i < 16; ++i) {
        iv[i] = 0;
    }
    aes_decrypt(key, iv, encrypted_string, decrypted_string, encrypted_length);
    printf("\n");
    printf("decrypted string = %s\n", decrypted_string);

}
