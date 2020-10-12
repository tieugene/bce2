#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "uintxxx.h"
#include "misc.h"
#include "base58.h"
#include "bech32.h"

using namespace std;

string      hash2hex(const uint256_t &h)
{
    auto u = reinterpret_cast<const uint64_t *> (&h);
    char tmp[65];
    tmp[64] = '\0';
    sprintf(tmp, "%016llx%016llx%016llx%016llx", u[3], u[2], u[1], u[0]);
    return string(tmp);
}

string      ripe2hex(const uint160_t &r)
{
    auto *u = reinterpret_cast<const uint32_t *>(&r);
    char tmp[41];
    tmp[40] = '\0';
    sprintf(tmp, "%08x%08x%08x%08x%08x", u[4], u[3], u[2], u[1], u[0]);
    return string(tmp);
}

void        __sha256(const void *src, const uint32_t size, uint256_t &dst)    ///< inner
{
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src, size);
    SHA256_Final(dst.begin(), &context);
}

void        hash256(const void *src, const uint32_t size, uint256_t &dst)
{
    uint256_t tmp;
    __sha256(src, size, tmp);
    __sha256(&tmp, sizeof(uint256_t), dst);
}

void        __ripe160(const uint256_t &src, uint8_t *dst)   ///< inner
{
    RIPEMD160_CTX context;
    RIPEMD160_Init(&context);
    RIPEMD160_Update(&context, reinterpret_cast<void const *>(&src), sizeof(uint256_t));
    RIPEMD160_Final(dst, &context);
}

void        hash160(const void *src, const uint32_t size, uint8_t *dst)
{
    uint256_t tmp;
    __sha256(src, size, tmp);
    __ripe160(tmp, dst);
}

// BIP-16
/** Convert datum hash160 into base58 encoded string */
string      ripe2addr(const uint8_t *src, const uint8_t pfx)
{
    uint8_t tmp1[sizeof(uint160_t)+5];
    tmp1[0] = pfx;
    memcpy(tmp1+1, src, sizeof (uint160_t));   // 1. add leading 0
    uint256_t tmp2;
    hash256(tmp1, sizeof (uint160_t)+1, tmp2);  // 2. 2 x sha256
    memcpy(tmp1+21, &(tmp2[0]), 4);             // 3. add crc
    return EncodeBase58(tmp1, tmp1+25);
}

/** Convert from one power-of-2 number base to another. */
// bitcoin-core/src/util/strencodings.h
template<int frombits, int tobits, bool pad, typename O, typename I>
bool __ConvertBits(const O& outfn, I it, I end) {
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

// BIP-173 (https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki)
std::string wpkh2addr(const uint8_t *v) {
    std::vector<unsigned char> data = {0};
    data.reserve(33);
    __ConvertBits<8, 5, true>([&](unsigned char c) { data.push_back(c); }, v, v + sizeof (uint160_t));
    return Bech32Encode(data);
}

std::string wsh2addr(const uint8_t *v) {
    std::vector<unsigned char> data = {0};
    data.reserve(53);
    __ConvertBits<8, 5, true>([&](unsigned char c) { data.push_back(c); }, v, v + sizeof (uint256_t));
    return Bech32Encode(data);
}
