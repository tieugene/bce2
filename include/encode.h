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
 * @brief Encode a Bech32 string
 * @param data Data to encode
 * @return Bech32 string
 * @note If hrp contains uppercase characters, this will cause an assertion error.
 */
std::string Bech32Encode(const u8vector &data);

#endif // ENCODE_H
