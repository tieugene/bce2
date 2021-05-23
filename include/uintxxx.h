#ifndef UINTXXX_H
#define UINTXXX_H

#include <string>
#include <vector>
#include "common.h"

/**
 * @brief Convert hash256 into hex string
 * @param src Data to hex
 * @return Hex string
 */
const std::string hash2hex(const uint256_t &src);                   // printers, handlers (err)
/**
 * @brief Convert hash160 into hex string
 * @param src Data to hex
 * @return Hex string
 */
const std::string ripe2hex(const uint160_t &src);                   // ---
/**
 * @brief Convert hash160 into Base58 address (BIP16)
 * @param src Data to convert
 * @param pfx Prefix (blockchain, 0=main)
 * @return Base58 address (bytes)
 */
const std::string ripe2addr(const u8_t *src, const u8_t pfx = 0);
inline const std::string ripe2addr(const uint160_t &src, const u8_t pfx = 0)
  { return ripe2addr((u8_t *) &src, pfx); }
/**
 * @brief Convert W0KEYHASH into Bech32 string
 * @note BIP-173 (https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki)
 * @param src Data to convert (uint160?)
 * @return Base32 string
 * @todo src: uchar* => &hash160
 * @note script
 */
const std::string wpkh2addr(const u8_t *src);                    // script
inline const std::string wpkh2addr(const uint160_t &src)
  { return wpkh2addr((u8_t *) &src); }
/**
 * @brief Convert W0SCRIPTHASH into Bech32 string
 * @param src Data to convert (uint256?)
 * @return Base32 string
 * @todo src: uchar* => string_view | &hash160
 */
const std::string wsh2addr(const u8_t *src);                     // script
inline const std::string wsh2addr(const uint256_t &src)
  { return wsh2addr((u8_t *) &src); }
/**
 * @brief Double SHA256 given pubkey
 * @param src Data to hash
 * @param size Data length
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
 * @todo string_view src
 */
void  hash160(const void *src, const uint32_t size, u8_t *dst);  // script
inline void  hash160(const void *src, const uint32_t size, uint160_t &dst)
  { hash160(src, size, (u8_t *) &dst); }

#endif // UINTXXX_H
