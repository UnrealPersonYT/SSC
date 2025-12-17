#include "sssed.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>   // for malloc, free

int main(void){
    // Sample key and nonce
    u32 key[_SSSED_KEYSIZE]     = {0x00112233,0x44556677,0x8899aabb,0xccddeeff,
                                   0x10203040,0x50607080,0x90a0b0c0,0xd0e0f000};
    u32 nonce[_SSSED_NONCE_SIZE] = {0x12345678, 0x9abcdef0, 0x0fedcba9, 0x87654321};

    // Allocate 1 GB buffer
    size_t buf_size = (size_t)1 << 30; // 1 GB = 2^30 bytes
    u8 *buffer = (u8*)malloc(buf_size);
    if (!buffer) {
        fprintf(stderr, "❌ Failed to allocate 1 GB buffer\n");
        return 1;
    }

    // Initialize buffer with some pattern (plaintext simulation)
    memset(buffer, 0x41, buf_size); // fill with 'A'

    printf("Encrypting 1 GB buffer...\n");

    // Encrypt
    _sssed_cipher(buffer, buf_size, key, nonce);

    // Write ciphertext to file
    FILE *f = fopen("ct.b", "wb");
    if (!f) {
        fprintf(stderr, "❌ Failed to open ct.b for writing\n");
        free(buffer);
        return 1;
    }
    fwrite(buffer, 1, buf_size, f);
    fclose(f);

    printf("✅ Ciphertext written to ct.b (1 GB)\n");

    // Clean up
    free(buffer);
    return 0;
}