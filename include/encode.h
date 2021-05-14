#ifndef ENCODE_H
#define ENCODE_H

#include <string_view>
#include <vector>

#include "common.h"

/**
 * @brief Encode into Base58
 * @param data Input data
 * @return Base58 encoded string
 * @note from bitcoin-core 0.19.1/src/base58
 */
std::string EncodeBase58(std::string_view data);

/**
 * Encode a Bech32 string. If hrp contains uppercase characters, this will cause an assertion error.
 */
std::string Bech32Encode(const std::vector<u8_t> &);

#endif // ENCODE_H
