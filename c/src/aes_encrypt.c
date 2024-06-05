#include "aes.h"
#include "string_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <hex_key> <hex_msg>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uint8_t key[AES_BLOCK_SIZE] = {0}; // 2b7e151628aed2a6abf7158809cf4f3c (test vector)
    uint8_t msg[AES_BLOCK_SIZE] = {0}; // 6bc1bee22e409f96e93d7e117393172a (test vector)
    uint8_t cip[AES_BLOCK_SIZE] = {0}; // 3ad77bb40d7a3660a89ecaf32466ef97 (test vector)

    hexload(key, AES_BLOCK_SIZE, argv[1]);
    hexload(msg, AES_BLOCK_SIZE, argv[2]);


    printf("Key: ");
    hexprint(stdout, key, AES_BLOCK_SIZE);
    putchar('\n');

    printf("Msg: ");
    hexprint(stdout, msg, AES_BLOCK_SIZE);
    putchar('\n');

    aes128_encrypt_block(cip, key, msg);

    printf("Cip: ");
    hexprint(stdout, cip, AES_BLOCK_SIZE);
    putchar('\n');

    return 0;
}
