/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <unordered_map>
#include <kcpolydb.h>
#include "uintxxx.h"

using namespace std;
//using namespace kyotocabinet;

typedef unordered_map<uint256_t, uint32_t> HashU32Map;  // unknown type uint256

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

class   TxMap_T {
    private:
        //HashU32Map  map;
    public:
        //TxMap_T() {}
        //bool        init() {}
        uint32_t    size(void);
        bool        add(uint256_t &, uint32_t);
        uint32_t    get(uint256_t &);
};

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
