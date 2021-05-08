#ifndef KV_TK_H
#define KV_TK_H

#ifdef TKRZW

#include <tkrzw_dbm_hash.h>
#include "kv/base.h"

// HashDBM
//const std::string TxFileName = "tx.tkh";
//const std::string AddrFileName = "addr.tkh";
// TinyDBM
//const std::string TxMemName = "tx.tkmt";
//const std::string AddrMemName = "addr.tkmt";
// TODO: try BabyDBM (.tkmb)

class KV_TK_T : public KV_BASE_T {
private:
    tkrzw::HashDBM  db;
public:
    bool        init(const string &);
    bool        close(void);
    void        clear(void);
    uint32_t    count(void);
    uint32_t    add(std::string_view key);
    uint32_t    add(const uint256_t &key)
                { return add(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get(std::string_view key);
    uint32_t    get(const uint256_t &key)
                { return get(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get_or_add(std::string_view key);
};

#endif //TKRZW

#endif // KV_TK_H
