/// @file 32.h
/// @brief 32-Bit Chunks Version
#pragma once
#include "stdint.h"

#ifdef __cplusplus
namespace ssc{
#endif
    /// @brief Constants Chosen Based On Uniform Bit Distribution And Hamming Distance
    #define _SSC_CONSTANTS (u32)0xDB4DA443, (u32)0xB62B4963, (u32)0xF256C239, (u32)0x68475CA7
    /// @brief        Simple Stream Cipher-32 Internal Round
    /// @param Chunk0 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    /// @param Chunk1 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    /// @param Chunk2 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    /// @param Chunk3 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    __declspec(noinline) void _ssc32_rnd(u32* const __restrict Chunk0, u32* const __restrict Chunk1, u32* const __restrict Chunk2, u32* const __restrict Chunk3){
        // Apply Add, Xor By Rotate, Add To Each 32 Bit Lane In A 128-Bit Chunk
        for(u64 Lane = 0; Lane < 4; ++Lane){
            // First Chunk
            Chunk0[Lane] += Chunk3[Lane];                                          // Add
            Chunk0[Lane] ^= (Chunk2[Lane] >> 19u) | (Chunk2[Lane] << (32u - 19u)); // Xor By Rotate
            Chunk0[Lane] += Chunk1[Lane];                                          // Add
            // Second Chunk
            Chunk1[Lane] += Chunk0[Lane];                                          // Add
            Chunk1[Lane] ^= (Chunk1[Lane] >> 17u) | (Chunk1[Lane] << (32u - 17u)); // Xor By Rotate
            Chunk1[Lane] += Chunk3[Lane];                                          // Add
            // Third Chunk
            Chunk2[Lane] += Chunk1[Lane];                                          // Add
            Chunk2[Lane] ^= (Chunk3[Lane] >> 13u) | (Chunk3[Lane] << (32u - 13u)); // Xor By Rotate
            Chunk2[Lane] += Chunk0[Lane];                                          // Add
            // Fourth Chunk
            Chunk3[Lane] += Chunk2[Lane];                                          // Add
            Chunk3[Lane] ^= (Chunk0[Lane] >> 7u)  | (Chunk0[Lane] << (32u - 7u));  // Xor By Rotate
            Chunk3[Lane] += Chunk2[Lane];                                          // Add
        }
    }
    /// @brief       Implementation Of Simple Stream Cipher-32
    /// @param Data  Data Pointer To Cipher
    /// @param Size  Total Size Of Data In Bytes
    /// @param Key   Pointer To 256-Bit Key
    /// @param Nonce Pointer To 128-Bit Nonce
    void _ssc32(u8* const __restrict Data, const u64 Size, const u32* const __restrict Key, const u32* const __restrict Nonce){
        u32 BaseStream[16] = {_SSC_CONSTANTS}; // 512-Bit Base Values
        {   // Initialize Base Stream
            // Load 256-Bit Key
            for(u64 DWord = 0; DWord < 8; ++DWord)
                BaseStream[DWord + 4] = Key[DWord];
            // Load 128-Bit Nonce
            for(u64 DWord = 0; DWord < 4; ++DWord)
                BaseStream[DWord + 12] = Nonce[DWord];
        }
        // 512-Bit Counter
        u32 Counter[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
                           9, 10, 11, 12, 13, 14, 15};
        // Go Through Data As Index
        for(u64 Index = 0; Index < Size;){
            // Get Total Chunk
            u64 ChunkSize = Size - Index; // Size Of Data Chunk
            // Limit Chunk To 512-Bits
            if(ChunkSize > 64)
                ChunkSize = 64;
            u32 PosStream[16]; // Positional 512-Bit Key Stream
            {   // Initialize Positional Stream
                for(u64 DWord = 0; DWord < 16; ++DWord){
                    // Load From Base Stream
                    PosStream[DWord] = BaseStream[DWord];
                    // Increment By Counter
                    PosStream[DWord] += Counter[DWord];
                    // Increment Counter
                    Counter[DWord] += 16u;
                }
            }
            // Do Rotations Of Internal Rotation
            for(u64 Rotations = 4; Rotations--;)
                _ssc32_rnd(&PosStream[0], &PosStream[4], &PosStream[8], &PosStream[12]);
            // Add BaseStream To Positional Stream
            for(u64 DWord = 0; DWord < 16; ++DWord)
                PosStream[DWord] += BaseStream[DWord];
            // Xor Data By Positional Stream
            for(u64 Byte = 0; Byte < ChunkSize; ++Byte)
                Data[Index + Byte] ^= ((u8*)PosStream)[Byte];
            // Increment Index
            Index += ChunkSize;
        }
    }
#ifdef __cplusplus
} // namespace ssc
#endif