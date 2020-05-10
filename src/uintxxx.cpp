#include <stdio.h>
//#include <iostream> // cerr
#include <cstring>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "uintxxx.h"
#include "misc.h"

using namespace std;

const char * const ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

string      EncodeBase58(uint8_t *src, size_t len) {
    auto dstlen = len * 137 / 100;
    char digits[dstlen], result[dstlen];
    size_t digitslen = 1;
    for (size_t i = 0; i < len; i++) {
        size_t carry = src[i];
        for (size_t j = 0; j < digitslen; j++) {
            carry += static_cast<size_t> (digits[j]) << 8;
            digits[j] = static_cast<char> (carry % 58);
            carry /= 58;
        }
        while (carry > 0) {
            digits[digitslen++] = static_cast<char> (carry % 58);
            carry /= 58;
        }
    }
    size_t resultlen = 0;
    // leading zero bytes
    for (; resultlen < len && src[resultlen] == 0;)
        result[resultlen++] = '1';
    // reverse
    for (size_t i = 0; i < digitslen; i++)
        result[resultlen + i] = ALPHABET[digits[digitslen - 1 - i]];
    result[digitslen + resultlen] = 0;
    return string(result);
}

string      hash2hex(uint256_t &h)
{
    void *v = static_cast<void *>(&h);
    uint64_t *u = static_cast<uint64_t *>(v);
    char tmp[65];
    tmp[64] = '\0';
    sprintf(tmp, "%016llx%016llx%016llx%016llx", u[3], u[2], u[1], u[0]);
    return string(tmp);
}

string      ripe2hex(uint160_t &r)
{
    void *v = static_cast<void *>(&r);
    uint32_t *u = static_cast<uint32_t *>(v);
    char tmp[41];
    tmp[40] = '\0';
    sprintf(tmp, "%08x%08x%08x%08x%08x", u[4], u[3], u[2], u[1], u[0]);
    return string(tmp);
}

/** Inner function */
void        sha256(void *src, uint32_t size, uint256_t &dst)
{
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src, size);
    SHA256_Final(dst.begin(), &context);
}

void        hash256(void *src, uint32_t size, uint256_t &dst)
{
    uint256_t tmp;
    sha256(src, size, tmp);
    sha256(&tmp, sizeof(uint256_t), dst);
}

/** Inner function */
void        ripe160(uint256_t &src, uint160_t &dst)
{
    RIPEMD160_CTX context;
    RIPEMD160_Init(&context);
    RIPEMD160_Update(&context, static_cast<void *>(&src), sizeof(uint256_t));
    RIPEMD160_Final(dst.begin(), &context);
}

void        hash160(void *src, uint32_t size, uint160_t &dst)
{
    uint256_t tmp;
    sha256(src, size, tmp);
    ripe160(tmp, dst);
}

/** Convert datum hash160 into base58 encoded string */
string      ripe2addr(uint160_t &src) {
    uint8_t tmp1[sizeof(uint160_t)+5];
    tmp1[0] = 0;
    memcpy(tmp1+1, &src, sizeof (uint160_t));   // 1. add leading 0
    uint256_t tmp2;
    hash256(tmp1, sizeof (uint160_t)+1, tmp2);  // 2. 2 x sha256
    memcpy(tmp1+21, &(tmp2[28]), 4);            // 3. add crc
    //cerr << "B4: " << ptr2hex(tmp1, 25) << endl;
    return EncodeBase58(tmp1, 25);
}

/* 2.
uint256_t   sha256(void *src, uint32_t size)
{
    uint256_t result;
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src, size);
    SHA256_Final(result.begin(), &context);
    return result;
}

uint256_t   sha256(uint256_t &src)
{
    uint256_t result;
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src.begin(), src.size());
    SHA256_Final(result.begin(), &context);
    return result;
}

uint256_t   hash256(void *src, uint32_t size)
{
    auto result = sha256(src, size);
    return sha256(result);
}
*/
