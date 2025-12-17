/// @file sssed.h
/// @brief Contains Main Cycle, Cipher, Defines, & Naming Conventions For S.S.E.C
#pragma once

#define _SSSED_TOTAL_CYCLES 4
#define _SSSED_KEYSIZE 8
#define _SSSED_NONCE_SIZE 4
#define _SSSED_KEYSTREAM_SIZE (_SSSED_KEYSIZE + _SSSED_NONCE_SIZE + 4)

typedef double    f64; typedef float              f32;
typedef long long i64; typedef unsigned long long u64; typedef signed long long s64;
typedef int       i32; typedef unsigned int       u32; typedef signed int       s32;
typedef short     i16; typedef unsigned short     u16; typedef signed short     s16;
typedef char       i8; typedef unsigned char       u8; typedef signed char       s8;

/// @brief Applies Addition, Xor By Rotation, & Add Over 4 Data Regions
void _sssed_cycle(u32 Reg0[_SSSED_KEYSTREAM_SIZE / 4], u32 Reg1[_SSSED_KEYSTREAM_SIZE / 4], u32 Reg2[_SSSED_KEYSTREAM_SIZE / 4], u32 Reg3[_SSSED_KEYSTREAM_SIZE / 4]){
    for(u32 i = 0; i < _SSSED_KEYSTREAM_SIZE / 4; ++i){
        // First 128-Bit Segment
        Reg0[i] += Reg2[i];
        Reg0[i] ^= (Reg3[i] >> 19) | (Reg3[i] << (32 - 19));
        Reg0[i] += Reg1[i];
        // Second 128-Bit Segment
        Reg1[i] += Reg3[i];
        Reg1[i] ^= (Reg0[i] >> 17) | (Reg0[i] << (32 - 17));
        Reg1[i] += Reg2[i];
        // Third 128-Bit Segment
        Reg2[i] += Reg0[i];
        Reg2[i] ^= (Reg1[i] >> 13) | (Reg1[i] << (32 - 13));
        Reg2[i] += Reg3[i];
        // Fourth 128-Bit Segment
        Reg3[i] += Reg1[i];
        Reg3[i] ^= (Reg2[i] >> 7) | (Reg2[i] << (32 - 7));
        Reg3[i] += Reg0[i];
    }
}

/// @brief       Simple.Symetric.Stream.Encrypt.Decrypt Cipher
/// @param Data  Pointer To Data To Cipher
/// @param Size  Size Of Data To Cipher
/// @param Key   Pointer To Key
/// @param Nonce Pointer To Nonce
void _sssed_cipher(u8* const __restrict Data, const u64 Size, const u32 Key[_SSSED_KEYSIZE], const u32 Nonce[_SSSED_NONCE_SIZE]){
    u32 ConstantKeystream[_SSSED_KEYSTREAM_SIZE]; // Constant Keystream
    {   // Initialize Keystream
        for(u32 i = 0; i < _SSSED_KEYSIZE; ++i)
            ConstantKeystream[i] = Key[i];
        for(u32 i = 0; i < _SSSED_NONCE_SIZE; ++i)
            ConstantKeystream[_SSSED_KEYSIZE + i] = Nonce[i];
        // Constants (Chosen Randomly May Change To Test (Must Be Non-Zero For Security))
        ConstantKeystream[_SSSED_KEYSTREAM_SIZE - 4] = 0xF17A8B92;
        ConstantKeystream[_SSSED_KEYSTREAM_SIZE - 3] = 0x6E9C4A8B;
        ConstantKeystream[_SSSED_KEYSTREAM_SIZE - 2] = 0x3B29A39F;
        ConstantKeystream[_SSSED_KEYSTREAM_SIZE - 1] = 0xE4A3BE2A;
    }
    // Positional Keystream
    u32 Keystream[_SSSED_KEYSTREAM_SIZE];
    // 128-Bit Positional Counter
    u32 Counter[4] = {0, 1, 2, 3};
    for(u64 Index = 0; Index < Size;){
        u64 ChunkSize = Size - Index;
        // Limit Size
        if(ChunkSize > _SSSED_KEYSTREAM_SIZE * 4)
            ChunkSize = _SSSED_KEYSTREAM_SIZE * 4;
        {   // Copy Constant
            for(u32 i = 0; i < _SSSED_KEYSTREAM_SIZE; ++i)
                Keystream[i] = ConstantKeystream[i];
        }
        {   // Xor By Counter
            for(u64 Dword = 0; Dword < _SSSED_KEYSTREAM_SIZE; ++Dword){
                Keystream[Dword] += Counter[Dword & 3];
                Counter[Dword & 3] += 4;
            }
        }
        {   // Cycle
            for(u64 Cycle = 0; Cycle < _SSSED_TOTAL_CYCLES; ++Cycle)
                _sssed_cycle(&Keystream[0],  &Keystream[_SSSED_KEYSTREAM_SIZE / 4],  &Keystream[_SSSED_KEYSTREAM_SIZE / 2],  &Keystream[_SSSED_KEYSTREAM_SIZE - (_SSSED_KEYSTREAM_SIZE / 4)]);
        }
        // Xor Data By Keystream
        for(u64 Byte = 0; Byte < ChunkSize; ++Byte)
            Data[Index + Byte] ^= ((u8*)Keystream)[Byte];
        // Increment Index
        Index += ChunkSize;
    }
}