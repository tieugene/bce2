#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "misc.h"
#include "crypt/uintxxx.h"
#include "crypt/encode.h"

using namespace std;

const string  ripe2addr(const uint160_t &src, const u8_t pfx) {  // was const u8_t *src
  // TODO: src (u8* => &u160)
  u8_t tmp1[sizeof(uint160_t)+5];
  tmp1[0] = pfx;
  memcpy(tmp1+1, src.begin(), sizeof (uint160_t));   // 1. add leading 0
  uint256_t tmp2;
  hash256(tmp1, sizeof (uint160_t)+1, tmp2);  // 2. 2 x sha256
  memcpy(tmp1+21, &(tmp2[0]), 4);             // 3. add crc
  return EncodeBase58(u8string_view(tmp1, 25));
}

template<int frombits, int tobits, bool pad, typename O, typename I>
/**
 * @brief Convert from one power-of-2 number base to another
 * @note Internal. Source: bitcoin-core/src/util/strencodings.h
 * @param outfn
 * @param it
 * @param end
 * @return
 */
bool  __ConvertBits(const O& outfn, I it, I end) {
  size_t acc = 0;
  size_t bits = 0;
  constexpr size_t maxv = (1 << tobits) - 1;
  constexpr size_t max_acc = (1 << (frombits + tobits - 1)) - 1;
  while (it != end) {
    acc = ((acc << frombits) | *it) & max_acc;
    bits += frombits;
    while (bits >= tobits) {
      bits -= tobits;
      outfn((acc >> bits) & maxv);
    }
    ++it;
  }
  if (pad) {
    if (bits) outfn((acc << (tobits - bits)) & maxv);
  } else if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
    return false;
  }
  return true;
}

const string  wpkh2addr(const uint160_t &src) {  // was const u8_t *v
  u8vector data = {0};
  data.reserve(33);
  __ConvertBits<8, 5, true>([&](unsigned char c) { data.push_back(c); }, src.begin(), src.end());
  return Bech32Encode(data);
}

const string  wsh2addr(const uint256_t &src) { // was const u8_t *src
  u8vector data = {0};
  data.reserve(53);
  __ConvertBits<8, 5, true>([&](unsigned char c) { data.push_back(c); }, src.begin(), src.end());
  return Bech32Encode(data);
}

/**
 * @brief Calculate SHA256
 * @note Internal helper for hash256()
 * @param src Data to hash
 * @param size Data size
 * @param dst Hash destination storage
 */
void  __sha256(const void *src, const uint32_t size, uint256_t &dst) {
  SHA256_CTX context;
  SHA256_Init(&context);
  SHA256_Update(&context, src, size);
  SHA256_Final(dst.begin(), &context);
}

void  hash256(const void *src, const uint32_t size, uint256_t &dst) {
  // TODO: src+size => string_view
  uint256_t tmp;
  __sha256(src, size, tmp);
  __sha256(&tmp, sizeof(uint256_t), dst);
}

/**
 * @brief Calculate RIPEMD160
 * @note Internal helper for hash160()
 * @param src Data to hash
 * @param dst Destination stroage
 */
void  __ripe160(const uint256_t &src, u8_t *dst) {
  RIPEMD160_CTX context;
  RIPEMD160_Init(&context);
  RIPEMD160_Update(&context, reinterpret_cast<void const *>(&src), sizeof(uint256_t));
  RIPEMD160_Final(dst, &context);
}

void  hash160(const void *src, const uint32_t size, uint160_t &dst) {
  // TODO: src+size => string_view
  uint256_t tmp;
  __sha256(src, size, tmp);
  __ripe160(tmp, (u8_t *) &dst);
}
