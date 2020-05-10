#ifndef UINTXXX_H
#define UINTXXX_H

#include <array>

using namespace std;

typedef array<uint8_t, 32> uint256_t;   ///< hash representation
typedef array<uint8_t, 20> uint160_t;   ///< addr representation

string      hash2hex(uint256_t &);
string      ripe2hex(uint160_t &);
string      ripe2addr(uint160_t &);

void        hash256(void *, uint32_t, uint256_t &); // 1.
void        hash160(void *, uint32_t, uint160_t &);

#endif // UINTXXX_H
