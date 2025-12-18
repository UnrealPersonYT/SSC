#include "sssed.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <size>\n", argv[0]);
        return 1;
    }

    // Parse argument as integer
    char *endptr;
    long N = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || N <= 0) {
        fprintf(stderr, "Error: argument must be a positive integer\n");
        return 1;
    }

    // Allocate buffer of N bytes, initialized to zero
    u8 *buffer = calloc(N, sizeof(u8));
    if (!buffer) {
        perror("calloc");
        return 1;
    }

    // Sample key and nonce
    u32 key[_SSSED_KEYSIZE] = {
        0x00112233,0x44556677,0x8899aabb,0xccddeeff,
        0x10203040,0x50607080,0x90a0b0c0,0xd0e0f000
    };
    u32 nonce[_SSSED_NONCE_SIZE] = {
        0x12345678, 0x9abcdef0, 0x0fedcba9, 0x87654321
    };

    // Encrypt
    _sssed_cipher(buffer, (u64)N, key, nonce);

    // Write ciphertext to file
    FILE *f = fopen("ct.b", "wb");
    if (!f) {
        perror("fopen");
        free(buffer);
        return 1;
    }
    fwrite(buffer, 1, N, f);
    fclose(f);
    printf("Written Ciphertext To ct.b For Further Analysis.\n");

    // Decrypt (same function)
    _sssed_cipher(buffer, (u64)N, key, nonce);

    // Verify buffer is all zeros
    int success = 1;
    for(long i = 0; i < N; i++){
        if(buffer[i] != 0){
            success = 0;
            break;
        }
    }
    if (success)
        printf("✅ Encrypt/Decrypt success: buffer restored to all zeros.\n");
    else
        printf("❌ Failure: buffer not restored to all zeros.\n");
    free(buffer);
    return success ? 0 : 1;
}