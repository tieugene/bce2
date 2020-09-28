#ifndef BASE58_H
#define BASE58_H

#include <stdint.h>
#include <string>

// from bitcoin-core 0.19.1/src/base58
// pbegin must be const...
std::string EncodeBase58(uint8_t*, const uint8_t*);

#endif // BASE58_H
