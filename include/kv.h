/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <kcpolydb.h>
#include <stdio.h>
#include "uintxxx.h"

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

// == common ==
class KV_T {
private:
    kyotocabinet::PolyDB     db;
protected:
    uint32_t    real_add(const uint8_t *, const uint16_t);
    uint32_t    real_get(const uint8_t *, const uint16_t);
public:
    bool        init(const string &s)
                { return db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE); }
    void        clear(void) { db.clear(); }
    uint32_t    count(void);
    bool        cpto(KV_T *, bool = false);
    bool        add(const string &key, const uint32_t value)
                { db.add(key, string((const char *)&value, sizeof(value))); return true; }
    uint32_t    add(const uint256_t &key)
                { return real_add(key.begin(), sizeof(uint256_t)); }
    uint32_t    add(const uint160_t &key)
                { return real_add(key.begin(), sizeof(uint160_t)); }
    uint32_t    add(const uint160_t key[], const uint8_t len)
                { return real_add(key[0].begin(), sizeof(uint160_t) * len); }
    uint32_t    get(const uint256_t &key)
                { return real_get(key.begin(), sizeof(uint256_t)); }
    uint32_t    get(const uint160_t &key)
                { return real_get(key.begin(), sizeof(uint160_t)); }
    uint32_t    get(const uint160_t key[], const uint8_t len)
                { return real_get(key[0].begin(), sizeof(uint160_t) * len); }
};

#endif // KV_H
