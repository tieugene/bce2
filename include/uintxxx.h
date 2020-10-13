#ifndef UINTXXX_H
#define UINTXXX_H

#include <cstdint>
#include <string>
#include <array>
//#include <algorithm>

template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

typedef byte_array<32> uint256_t;   ///< hash representation
typedef byte_array<20> uint160_t;   ///< addr representation

//bool cmp_uint160(const uint160_t &l, const uint160_t &r)
//    { return memcmp(&l, &r, sizeof (uint160_t)) < 0; }

std::string     hash2hex(const uint256_t &);
std::string     ripe2hex(const uint160_t &);
std::string     ripe2addr(const uint8_t *, const uint8_t = 0);
std::string     wpkh2addr(const uint8_t *);
std::string     wsh2addr(const uint8_t *);

void        hash256(const void *, const uint32_t, uint256_t &); // 1.
void        hash160(const void *, const uint32_t, uint8_t *);

#endif // UINTXXX_H
