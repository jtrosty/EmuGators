#pragma once

/* For common inline definitions */

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using byte = u8;

using i8 = char;
using i16 = short;


#define ALWAYS_INLINE inline __attribute__((always_inline))

template<typename T>
constexpr T kib(T num) { return num * 1024; }

