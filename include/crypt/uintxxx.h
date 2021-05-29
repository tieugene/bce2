#ifndef UINTXXX_H
#define UINTXXX_H

#include <string>
#include <vector>
#include "common.h"

/**
 * @brief Convert hash160 into Base58 address (BIP16)
 * @param src Data to convert
 * @param pfx Prefix (blockchain, 0=main)
 * @return Base58 address (bytes)
 */
const std::string ripe2addr(const uint160_t &src, const u8_t pfx = 0);
/**
 * @brief Convert W0KEYHASH into Bech32 string
 * @note BIP-173 (https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki)
 * @param src Data to convert (uint160?)
 * @return Base32 string
 * @todo src: uchar* => &hash160
 * @note script
 */
const std::string wpkh2addr(const uint160_t &src);
/**
 * @brief Convert W0SCRIPTHASH into Bech32 string
 * @param src Data to convert (uint256)
 * @return Base32 string
 * @todo src: uchar* => string_view | &hash160
 */
const std::string wsh2addr(const uint256_t &src);
/**
 * @brief Double SHA256 given pubkey
 * @param src Data to hash
 * @param size Data length
 * @note bk, tx
 * @todo string_view src
 */
void  hash256(const void *src, const uint32_t size, uint256_t &);
inline void  hash256(std::string_view src, uint256_t &dst) { hash256(src.begin(), src.length(), dst); }
// inline void  hash256(std::vector<char> &src, uint256_t &dst) { hash256(src.begin(), src.size(), dst); }
/**
 * @brief Hash address (SHA256 | RIPE160)
 * @param src Hashed data
 * @param size Data length
 * @param dst Where result put to
 * @note addr.*
 */
void  hash160(const void *src, const uint32_t size, uint160_t &dst);

#endif // UINTXXX_H
