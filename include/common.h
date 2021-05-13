#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <array>
#include <string_view>
#include <stdexcept>

const uint32_t MAX_UINT32 = 0xFFFFFFFF;
typedef unsigned char u8_t;
template <size_t Size>
using byte_array = std::array<u8_t, Size>;
typedef byte_array<32> uint256_t;   ///< hash representation
typedef byte_array<20> uint160_t;   ///< addr representation
inline std::string_view u8string_view(const u8_t *s, size_t size) {
  return std::string_view((const char *) s, size);
}

union   UNIPTR_T    ///< Universal ptr
{
    const void        *v_ptr;
    //const char        *ch_ptr;
    const u8_t        *u8_ptr;
    const uint16_t    *u16_ptr;
    const uint32_t    *u32_ptr;
    const uint64_t    *u64_ptr;
    const uint160_t   *u160_ptr;
    const uint256_t   *u256_ptr;
    UNIPTR_T() {}
    UNIPTR_T(const uint160_t * ptr) { u160_ptr = ptr; }
    UNIPTR_T(const uint256_t * ptr) { u256_ptr = ptr; }
    uint32_t    take_32(void) { return *u32_ptr++; }
    uint64_t    take_64(void) { return *u64_ptr++; }
    uint32_t    take_varuint(void);
    const u8_t        *take_u8_ptr(const uint32_t);
    const uint32_t    *take_32_ptr(void) { return u32_ptr++; }
    const uint256_t   *take_256_ptr(void) { return u256_ptr++; }
};

class BCException : public std::runtime_error {
public:
  BCException(const std::string &msg) : std::runtime_error(msg) {}
  BCException(const char *msg) : std::runtime_error(msg) {}
};

// error shortcuts
bool b_error(const std::string &);
u_int32_t u32_error(const std::string &, uint32_t = MAX_UINT32);

#endif // COMMON_H
