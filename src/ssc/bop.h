/// @file bop.h
/// @brief Contains Utilities For Bitwise Operations
#pragma once
#include "stdint.h"

#ifdef __cplusplus
namespace ssc{
#endif
    /// @brief          Returns A Right Rotated 32-Bit Integer
    /// @param Base     Number To Rotate
    /// @param Rotation Amount To Rotate By
    /// @return         Rotated Number
    u32 ror32(u32 Base, u32 Rotation){
        return (Base >> Rotation) | (Base << (32u - Rotation));
    }
    /// @brief          Returns A Left Rotated 32-Bit Integer
    /// @param Base     Number To Rotate
    /// @param Rotation Amount To Rotate By
    /// @return         Rotated Number
    u32 rol32(u32 Base, u32 Rotation){
        return (Base << Rotation) | (Base >> (32u - Rotation));
    }
#ifdef __cplusplus
} // namespace ssc
#endif