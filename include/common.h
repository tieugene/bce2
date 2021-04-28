#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <array>

template <size_t Size>
using byte_array = std::array<uint8_t, Size>;
typedef byte_array<32> uint256_t;   ///< hash representation
typedef byte_array<20> uint160_t;   ///< addr representation
union   UNIPTR_T    ///< Universal ptr
{
    const void        *v_ptr;
    const uint8_t     *u8_ptr;
    const uint16_t    *u16_ptr;
    const uint32_t    *u32_ptr;
    const uint64_t    *u64_ptr;
    const uint160_t   *u160_ptr;
    const uint256_t   *u256_ptr;
    UNIPTR_T() {}
    UNIPTR_T(const uint160_t * ptr) { u160_ptr = ptr; }
    UNIPTR_T(const uint256_t * ptr) { u256_ptr = ptr; }
};

#endif // COMMON_H
