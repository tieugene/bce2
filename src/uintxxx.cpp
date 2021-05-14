#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "uintxxx.h"
#include "misc.h"
#include "encode.h"

using namespace std;

const string  hash2hex(const uint256_t &h) {
  UNIPTR_T u(&h);
  char tmp[65];
  tmp[64] = '\0';
  sprintf(tmp,
#if defined(__APPLE__)
    "%016llx%016llx%016llx%016llx",
#else
    "%016lx%016lx%016lx%016lx",
#endif
    u.u64_ptr[3], u.u64_ptr[2], u.u64_ptr[1], u.u64_ptr[0]);
  return string(tmp);
}

const string  ripe2hex(const uint160_t &r) {
  UNIPTR_T u(&r);
  char tmp[41];
  tmp[40] = '\0';
  sprintf(tmp, "%08x%08x%08x%08x%08x", u.u32_ptr[4], u.u32_ptr[3], u.u32_ptr[2], u.u32_ptr[1], u.u32_ptr[0]);
  return string(tmp);
}

const string  ripe2addr(const u8_t *src, const u8_t pfx) {
  u8_t tmp1[sizeof(uint160_t)+5];
  tmp1[0] = pfx;
  memcpy(tmp1+1, src, sizeof (uint160_t));   // 1. add leading 0
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

const string  wpkh2addr(const u8_t *v) {
  u8vector data = {0};
  data.reserve(33);
  __ConvertBits<8, 5, true>([&](unsigned char c) { data.push_back(c); }, v, v + sizeof (uint160_t));
  return Bech32Encode(data);
}

const string  wsh2addr(const u8_t *v) {
  u8vector data = {0};
  data.reserve(53);
  __ConvertBits<8, 5, true>([&](unsigned char c) { data.push_back(c); }, v, v + sizeof (uint256_t));
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

void  hash160(const void *src, const uint32_t size, u8_t *dst) {
  uint256_t tmp;
  __sha256(src, size, tmp);
  __ripe160(tmp, dst);
}
