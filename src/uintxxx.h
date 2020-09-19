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

std::string      hash2hex(uint256_t const &);
std::string      ripe2hex(uint160_t const &);
std::string      ripe2addr(uint160_t const &);

void        hash256(void const *, uint32_t const, uint256_t &); // 1.
void        hash160(void const *, uint32_t const, uint160_t &);

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
