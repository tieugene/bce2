/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <string_view>
#include "uintxxx.h"
#ifdef TKRZW
#include <tkrzw_dbm_poly.h>
const std::string TxFileName = "tx.tkh";
const std::string AddrFileName = "addr.tkh";
const std::string TxMemName = "tx.tmtb";
const std::string AddrMemName = "addr.tmtb";
#else
#include <kcpolydb.h>
const std::string TxFileName = "tx.kch";
const std::string AddrFileName = "addr.kch";
const std::string TxMemName = ":";
const std::string AddrMemName = ":";
#endif

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

// == common ==
class KV_T {
private:
#ifdef TKRZW
  tkrzw::PolyDBM  db;
#else
    kyotocabinet::PolyDB     db;
    bool        opened = false;
#endif
public:
    bool        init(const string &);
    bool        close(void);
    void        clear(void);
    uint32_t    count(void);
    bool        cpto(KV_T *);
    bool        add(const string &key, const uint32_t value);
    uint32_t    add_raw(const uint8_t *, const uint16_t);
    uint32_t    add(const uint256_t &key)                     // tx, WSH
                { return add_raw(key.begin(), sizeof(uint256_t)); }
    uint32_t    get_raw(const uint8_t *, const uint16_t);
    uint32_t    get(const uint256_t &key)
                { return get_raw(key.begin(), sizeof(uint256_t)); }
};

#endif // KV_H
