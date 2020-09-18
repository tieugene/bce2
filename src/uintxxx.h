#ifndef UINTXXX_H
#define UINTXXX_H

#include <cstdint>
#include <string>
#include <array>
#include <boost/functional/hash.hpp>

template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

typedef byte_array<32> uint256_t;   ///< hash representation
typedef byte_array<20> uint160_t;   ///< addr representation

std::string      hash2hex(uint256_t &);
std::string      ripe2hex(uint160_t &);
std::string      ripe2addr(uint160_t &);

void        hash256(void *, uint32_t, uint256_t &); // 1.
void        hash160(void *, uint32_t, uint160_t &);

namespace std
{
template <size_t Size>
struct hash<byte_array<Size>>
{
    size_t operator()(const byte_array<Size>& hash) const
    {
        return boost::hash_range(hash.begin(), hash.end());
    }
};
}// namespace std

namespace boost
{
template <size_t Size>
struct hash<byte_array<Size>>
{
    size_t operator()(const byte_array<Size>& hash) const
    {
        return boost::hash_range(hash.begin(), hash.end());
    }
};
} // namespace boost

#endif // UINTXXX_H
