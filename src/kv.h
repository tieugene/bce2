/** Key-value storage */
// TODO: k-v template
#ifndef KV_H
#define KV_H

#include <kcpolydb.h>
#include "uintxxx.h"

using namespace std;
//using namespace kyotocabinet;

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

class   KVDB_T {
protected:
    kyotocabinet::PolyDB     db;
public:
    bool        init(string &);
    uint32_t    size(void);
};

class   TxDB_T : public KVDB_T {
public:
    uint32_t    add(uint256_t &);
    uint32_t    get(uint256_t &);
};

class   AddrDB_T : public KVDB_T {
public:
    uint32_t    add(uint160_t &);
    uint32_t    get(uint160_t &);
};

#endif // KV_H
