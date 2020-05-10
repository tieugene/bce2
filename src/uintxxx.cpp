#include <stdio.h>
#include <openssl/sha.h>
#include "uintxxx.h"

// 1.
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

string      hash2hex(uint256_t &h)
{
    void *v = static_cast<void *>(&h);
    uint64_t *u = static_cast<uint64_t *>(v);
    char tmp[257];
    tmp[256] = '\0';
    sprintf(tmp, "%016llx%016llx%016llx%016llx", u[3], u[2], u[1], u[0]);
    return string(tmp);
}
