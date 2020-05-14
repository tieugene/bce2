/** Key-value storage */
// TODO: k-v template
#ifndef KV_H
#define KV_H

#include <kcpolydb.h>
#include "uintxxx.h"

using namespace std;
//using namespace kyotocabinet;

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

class   TxDB_T {
private:
    kyotocabinet::PolyDB     db;
public:
    bool        init(string &);
    uint64_t    size(void);
    bool        add(uint256_t &, uint32_t);
    uint32_t    get(uint256_t &);
};

#endif // KV_H
