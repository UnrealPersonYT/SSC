#include "sssed.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
u64 get_time_ns(void){
    static LARGE_INTEGER freq;
    static i32 init = 0;
    if(!init){ QueryPerformanceFrequency(&freq); init = 1; }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (u64)(counter.QuadPart * 1000000000ull / freq.QuadPart);
}
#else
#include <time.h>
u64 get_time_ns(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (u64)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}
#endif

i32 main(i32 argc, i8 *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: %s <size>\n", argv[0]);
        return 1;
    }
    // Parse argument as integer
    i8 *endptr;
    i32 N = strtol(argv[1], &endptr, 10);
    if(*endptr != '\0' || N <= 0){
        fprintf(stderr, "Error: argument must be a positive integer\n");
        return 1;
    }
    // Allocate buffer of N bytes, initialized to zero
    u8 *buffer = calloc(N, sizeof(u8));
    if(!buffer){
        perror("calloc");
        return 1;
    }
    printf("Info:\nCycles(%llu)\nKeySize(%llu)\nNonceSize(%llu)\n\n", (u64)_SSSED_TOTAL_CYCLES, (u64)_SSSED_KEYSIZE, (u64)_SSSED_NONCE_SIZE);
    // Sample key and nonce
    u32 key[_SSSED_KEYSIZE] = {0};
    u32 nonce[_SSSED_NONCE_SIZE] = {0};
    // Encrypt timing
    u64 start = get_time_ns();
    _sssed_cipher(buffer, (u64)N, key, nonce);
    u64 end = get_time_ns();
    double elapsed_ms = (end - start) / 1e6;
    printf("Encryption took %.3f ms (%.2f MB/s)\n",
           elapsed_ms, (N / 1e6) / (elapsed_ms / 1000.0));
    // Write ciphertext to file
    FILE *f = fopen("ct.b", "wb");
    if(!f){
        perror("fopen");
        free(buffer);
        return 1;
    }
    fwrite(buffer, 1, N, f);
    fclose(f);
    printf("Written Ciphertext To ct.b For Further Analysis.\n");
    // Decrypt timing
    start = get_time_ns();
    _sssed_cipher(buffer, (u64)N, key, nonce);
    end = get_time_ns();
    elapsed_ms = (end - start) / 1e6;
    printf("Decryption took %.3f ms (%.2f MB/s)\n\n",
           elapsed_ms, (N / 1e6) / (elapsed_ms / 1000.0));
    // Verify buffer is all zeros
    i32 success = 1;
    for(u64 i = 0; i < N; i++){
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