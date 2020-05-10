#ifndef UINTXXX_H
#define UINTXXX_H

#include <array>

using namespace std;

typedef array<uint8_t, 32> uint256_t;   ///< hash representation
typedef array<uint8_t, 20> uint160_t;   ///< addr representation
//typedef uint8_t[32] uint256_t;   ///< hash representation

void        hash256(void *, uint32_t, uint256_t &); // 1.
//uint256_t   hash256(void *, uint32_t);            // 2.

string      hash2hex(uint256_t &);

#endif // UINTXXX_H
