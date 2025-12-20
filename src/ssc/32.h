/// @file 32.h
/// @brief 32-Bit Chunks Version
#pragma once
#include "bop.h"

#ifdef __cplusplus
namespace ssc{
#endif
    /// @brief Constants Chosen Based On Uniform Bit Distribution And Hamming Distance
    #define _SSC_CONSTANTS (u32)0xA3F19C5E, (u64)0x5C8AE7B1, (u64)0x9E47D0A3, (u64)0x71B2F58C
    /// @brief Total Rounds Per Call (Change For Test Purposes)
    #define _SCC_ROUNDS 4
    /// @brief        Simple Stream Cipher-32 Internal Round
    /// @param Chunk0 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    /// @param Chunk1 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    /// @param Chunk2 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    /// @param Chunk3 128-Bit Chunk Consisting Of 4 32-Bit Lanes
    void _ssc32_rnds(u32* const __restrict Chunk0, u32* const __restrict Chunk1, u32* const __restrict Chunk2, u32* const __restrict Chunk3){
        for(u64 Rounds = _SCC_ROUNDS; Rounds--;){  // Do Multiple Rounds
            // Apply Add, Xor By Rotate, Add To Each 32 Bit Lane In A 128-Bit Chunk
            for(u64 Lane = 0; Lane < 4; ++Lane){
                // First Chunk
                Chunk0[Lane] += Chunk1[Lane];                       // Add
                Chunk0[Lane] ^= ror32(Chunk3[(Lane + 1) & 3], 19u); // Xor By Rotate (Different Chunk)
                Chunk0[Lane] += Chunk2[(Lane + 2) & 3];             // Add
                Chunk0[Lane] ^= ror32(Chunk0[(Lane + 3) & 3], 7u);  // Xor By Rotate (Same Chunk)
                // Second Chunk
                Chunk1[Lane] += Chunk2[Lane];                       // Add
                Chunk1[Lane] ^= ror32(Chunk0[(Lane + 1) & 3], 17u); // Xor By Rotate (Different Chunk)
                Chunk1[Lane] += Chunk3[(Lane + 2) & 3];             // Add
                Chunk1[Lane] ^= ror32(Chunk1[(Lane + 3) & 3], 13u); // Xor By Rotate (Same Chunk)
                // Third Chunk
                Chunk2[Lane] += Chunk3[Lane];                       // Add
                Chunk2[Lane] ^= ror32(Chunk1[(Lane + 1) & 3], 13u); // Xor By Rotate (Different Chunk)
                Chunk2[Lane] += Chunk0[(Lane + 2) & 3];             // Add
                Chunk2[Lane] ^= ror32(Chunk2[(Lane + 3) & 3], 17u); // Xor By Rotate (Same Chunk)
                // Fourth Chunk
                Chunk3[Lane] += Chunk0[Lane];                       // Add
                Chunk3[Lane] ^= ror32(Chunk2[(Lane + 1) & 3], 7u);  // Xor By Rotate
                Chunk3[Lane] += Chunk1[(Lane + 2) & 3];             // Add
                Chunk3[Lane] ^= ror32(Chunk3[(Lane + 3) & 3], 19u); // Xor By Rotate (Same Chunk)
            }
        }
    }
    /// @brief       Implementation Of Simple Stream Cipher-32
    /// @param Data  Data Pointer To Cipher
    /// @param Size  Total Size Of Data In Bytes
    /// @param Key   Pointer To 256-Bit Key
    /// @param Nonce Pointer To 128-Bit Nonce
    void _ssc32(u8* const __restrict Data, const u64 Size, const u32* const __restrict Key, const u32* const __restrict Nonce){
        u32 BaseStream[16] = {_SSC_CONSTANTS, Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Nonce[0], Nonce[1], Nonce[2], Nonce[3]}; // 512-Bit Base Values
        // Go Through Data As Index
        for(u64 Index = 0; Index < Size;){
            // Get Total Chunk
            u64 ChunkSize = Size - Index; // Size Of Data Chunk
            // Limit Chunk To 512-Bits
            if(ChunkSize > 64)
                ChunkSize = 64;
            u32 PosStream[16]; // Positional 512-Bit Key Stream
            {   // Initialize Positional Stream
                for(u64 DWord = 0; DWord < 16; ++DWord)
                    // Load From Base Stream + Index + Position In File
                    PosStream[DWord] = BaseStream[DWord];
                for(u64 DWord = 0; DWord < 4; ++DWord)
                    // Increment Constants
                    BaseStream[DWord] += ChunkSize;
            }
            // Do Internal Rounds
            _ssc32_rnds(&PosStream[0], &PosStream[4], &PosStream[8], &PosStream[12]);
            // Add BaseStream To Positional Stream
            for(u64 DWord = 0; DWord < 16; ++DWord)
                PosStream[DWord] += BaseStream[DWord];
            // Xor Data By Positional Stream
            memxor(&Data[Index], (u8*)PosStream, ChunkSize);
            // Increment Index
            Index += ChunkSize;
        }
    }
#ifdef __cplusplus
} // namespace ssc
#endif