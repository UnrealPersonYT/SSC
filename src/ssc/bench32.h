/// @file bench32.h
/// @brief Contains Utilities For Benchmarking SSC-32 On Your Device
#pragma once
#include "32.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
namespace ssc{
#endif
    /// @brief Benchmarks SSC-32 On Your Device & Prints Output w/ printf
    void _ssc32_bench(void){
        printf("Starting Bench-32...\nThe SSC-32 Benchmarker...\n");
        // Ready Benchmark Values
        u64 Size = 1ull << 30ull;
        clock_t Start, End;
        // Zero Key & Nonce
        u32 Key[8]   = {0};
        u32 Nonce[4] = {0};
        printf("Benchmark Values Ready...\nValues:\n    Size(%llu)\nAllocating Test Data w/ Size\n", Size);
        // Start Allocation
        Start = clock();
        u8* TestData = (u8*)calloc(Size, 1);
        // End Allocation
        End = clock();
        printf("Allocation Time Taken(%llu *Clock Ticks*)\n", End - Start);
        // Start Encrytion
        Start = clock();
        _ssc32(TestData, Size, Key, Nonce);
        // End Encryption
        End = clock();
        printf("Encryption Time Taken(%llu *Clock Ticks*)\nBytes Ciphered Per Tick(%.3f)\n", End - Start, (f64)Size / ((f64)End - (f64)Start));
        // Start Decryption
        Start = clock();
        _ssc32(TestData, Size, Key, Nonce);
        // End Decryption
        End = clock();
        printf("Decryption Time Taken(%llu *Clock Ticks*)\nBytes Ciphered Per Tick(%.3f)\n", End - Start, (f64)Size / ((f64)End - (f64)Start));
        printf("Checking If Data Decrypted Correctly...\n");
        u8 Passed = 1;
        for(u64 Byte = 0; Byte < Size; ++Byte)
            if(TestData[Byte]){
                Passed = 0;
                break;
            }
        if(Passed)
            printf("✅ Success\n");
        else
            printf("❌ Failed\n");
        printf("Freeing Test Data\n");
        // Start Freeing
        Start = clock();
        free(TestData);
        // End Freeing
        End = clock();
        printf("Freeing Time Taken(%llu *Clock Ticks*)\n", End - Start);
    }
#ifdef __cplusplus
} // namespace ssc
#endif