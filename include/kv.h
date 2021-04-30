/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <string_view>
#include "common.h"
#ifdef TKRZW
#include <tkrzw_dbm_poly.h>
// HashDBM
const std::string TxFileName = "tx.tkh";
const std::string AddrFileName = "addr.tkh";
// TinyDBM
const std::string TxMemName = "tx.tkmt";
const std::string AddrMemName = "addr.tkmt";
// TODO: BabyDBM (.tkmb)
#else
#include <kcpolydb.h>
// HashDB
const std::string TxFileName = "tx.kch";
const std::string AddrFileName = "addr.kch";
// StashDB
const std::string TxMemName = ":";
const std::string AddrMemName = ":";
#endif

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

bool    set_cache(void); ///< setup k-v storages
void    stop_cache(void); ///< reset k-v storages

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
    //bool        add(const string &key, const uint32_t value);
    /**
     * @brief Add new k-v pair
     * @param key Key to add
     * @return Value of new key added or NOT_FOUND_U32
     */
    uint32_t    add(std::string_view key);
    uint32_t    add(const uint256_t &key)                     // tx, WSH
                { return add(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    /**
     * @brief Get value of key
     * @param key Key to find
     * @return Key value or NOT_FOUND_U32
     */
    uint32_t    get(std::string_view key);
    uint32_t    get(const uint256_t &key)
                { return get(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    /**
     * @brief Try to get existing k-v or add new
     * @param key Key to find
     * @return Found or added value or NOT_FOUND_U32 on error (not found nor added)
     * @throw Not found nor added, added as not expected
     */
    uint32_t    get_or_add(std::string_view key);
};

#endif // KV_H
