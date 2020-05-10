#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "uintxxx.h"


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
