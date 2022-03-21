/**
 * @file   types.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Data types redefinition from C++ standard inside a namespace.
 *         Should only be use where it make sense to rename the data types.
 * @date   2022-03-21
 *
 * @copyright Copyright (c) 2022
 */
#pragma once
#include <cstdint>
#include <cstddef>

namespace nerv {
using f32 = float;          // float 32-bit
using f64 = double;         // float 64-bit

using u8  = std::uint8_t;   // unsigned integer  8-bit
using u16 = std::uint16_t;  // unsinged integer 16-bit
using u32 = std::uint32_t;  // unsigned integer 32-bit
using u64 = std::uint64_t;  // unsinged integer 64-bit

using i8  = std::int8_t;    // integer  8-bit
using i16 = std::int16_t;   // integer 16-bit
using i32 = std::int32_t;   // integer 32-bit
using i64 = std::int64_t;   // integer 64-bit

using usize = std::size_t;     // unsigned arch
using isize = std::ptrdiff_t;  // signed   arch

namespace max {
u8  u8  = UINT8_MAX;
u16 u16 = UINT16_MAX;
u32 u32 = UINT32_MAX;
u64 u64 = UINT64_MAX;

i8  i8  = INT8_MAX;
i16 i16 = INT16_MAX;
i32 i32 = INT32_MAX;
i64 i64 = INT64_MAX;

usize usize = SIZE_MAX;
isize isize = PTRDIFF_MAX;
}

namespace min {
i8  i8  = INT8_MIN;
i16 i16 = INT16_MIN;
i32 i32 = INT32_MIN;
i64 i64 = INT64_MIN;

isize isize = PTRDIFF_MIN;
}
}

