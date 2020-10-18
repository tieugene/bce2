/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <kcpolydb.h>
#include "uintxxx.h"

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

// == common ==
class KV_T {
private:
    kyotocabinet::PolyDB     db;
public:
    bool        init(const string &);
    void        clear(void) { db.clear(); }
    uint32_t    count(void);
    bool        cpto(KV_T *);
    bool        add(const string &key, const uint32_t value)
                { db.add(key, string((const char *)&value, sizeof(value))); return true; }
    uint32_t    add_raw(const uint8_t *, const uint16_t);
    uint32_t    add(const uint256_t &key)                     // tx, WSH
                { return add_raw(key.begin(), sizeof(uint256_t)); }
    uint32_t    get_raw(const uint8_t *, const uint16_t);
    uint32_t    get(const uint256_t &key)
                { return get_raw(key.begin(), sizeof(uint256_t)); }
};

#endif // KV_H
