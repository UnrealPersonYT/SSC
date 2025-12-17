
#include "sssed.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void){
    // Sample key and nonce
    u32 key[_SSSED_KEYSIZE]     = {0x00112233,0x44556677,0x8899aabb,0xccddeeff,0x10203040,0x50607080,0x90a0b0c0,0xd0e0f000};
    u32 nonce[_SSSED_NONCE_SIZE] = {0x12345678, 0x9abcdef0, 0x0fedcba9, 0x87654321};

    // Sample plaintext
    char plaintext[] = "Hello, SSSED! This is a test of the cipher.";
    u8 buffer[sizeof(plaintext)];
    memcpy(buffer, plaintext, sizeof(plaintext));

    printf("Original: %s\n", buffer);

    // Encrypt
    _sssed_cipher(buffer, sizeof(plaintext), key, nonce);
    printf("Encrypted (hex): ");
    for(u64 i = 0; i < sizeof(plaintext); ++i)
        printf("%02X ", buffer[i]);
    printf("\n");

    // Decrypt (same function)
    _sssed_cipher(buffer, sizeof(plaintext), key, nonce);
    printf("Decrypted: %s\n", buffer);

    // Verify
    if(memcmp(buffer, plaintext, sizeof(plaintext)) == 0)
        printf("✅ Success: Decrypted text matches original.\n");
    else
        printf("❌ Failure: Decrypted text does not match original.\n");
    return 0;
}