#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "uintxxx.h"
#include "misc.h"

using namespace std;

static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

string EncodeBase58(uint8_t* pbegin, const uint8_t* pend)    // bitcoin-core 0.19.1; pbegin must be const...
{
    // Skip & count leading zeroes.
    int zeroes = 0;
    int length = 0;
    while (pbegin != pend && *pbegin == 0) {
        pbegin++;
        zeroes++;
    }
    // Allocate enough space in big-endian base58 representation.
    int size = (pend - pbegin) * 138 / 100 + 1; // log(256) / log(58), rounded up.
    vector<unsigned char> b58(size);
    // Process the bytes.
    while (pbegin != pend) {
        int carry = *pbegin;
        int i = 0;
        // Apply "b58 = b58 * 256 + ch".
        for (std::vector<unsigned char>::reverse_iterator it = b58.rbegin(); (carry != 0 || i < length) && (it != b58.rend()); it++, i++) {
            carry += 256 * (*it);
            *it = carry % 58;
            carry /= 58;
        }

        assert(carry == 0);
        length = i;
        pbegin++;
    }
    // Skip leading zeroes in base58 result.
    vector<unsigned char>::iterator it = b58.begin() + (size - length);
    while (it != b58.end() && *it == 0)
        it++;
    // Translate the result into a string.
    string str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, '1');
    while (it != b58.end())
        str += pszBase58[*(it++)];
    return str;
}

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

/** Inner function */
void        sha256(const void *src, const uint32_t size, uint256_t &dst)
{
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src, size);
    SHA256_Final(dst.begin(), &context);
}

void        hash256(const void *src, const uint32_t size, uint256_t &dst)
{
    uint256_t tmp;
    sha256(src, size, tmp);
    sha256(&tmp, sizeof(uint256_t), dst);
}

/** Inner function */
void        ripe160(const uint256_t &src, uint160_t &dst)
{
    RIPEMD160_CTX context;
    RIPEMD160_Init(&context);
    RIPEMD160_Update(&context, reinterpret_cast<void const *>(&src), sizeof(uint256_t));
    RIPEMD160_Final(dst.begin(), &context);
}

void        hash160(const void *src, const uint32_t size, uint160_t &dst)
{
    uint256_t tmp;
    sha256(src, size, tmp);
    ripe160(tmp, dst);
}

/** Convert datum hash160 into base58 encoded string */
string      ripe2addr(const uint160_t &src, const uint8_t pfx)
{
    uint8_t tmp1[sizeof(uint160_t)+5];
    tmp1[0] = pfx;
    memcpy(tmp1+1, &src, sizeof (uint160_t));   // 1. add leading 0
    uint256_t tmp2;
    hash256(tmp1, sizeof (uint160_t)+1, tmp2);  // 2. 2 x sha256
    memcpy(tmp1+21, &(tmp2[0]), 4);             // 3. add crc
    return EncodeBase58(tmp1, tmp1+25);
}
