#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <array>

template <size_t Size>
using byte_array = std::array<uint8_t, Size>;
typedef byte_array<32> uint256_t;   ///< hash representation
typedef byte_array<20> uint160_t;   ///< addr representation
union MULTIPTR_T {
    const uint8_t *u8;
    const uint16_t *u16;
    const uint32_t *u32;
    const uint64_t *u64;
    const uint160_t *u160;
    const uint256_t *u256;
    MULTIPTR_T() {}
    MULTIPTR_T(const uint160_t * ptr) { u160 = ptr; }
    MULTIPTR_T(const uint256_t * ptr) { u256 = ptr; }
};

#endif // COMMON_H
