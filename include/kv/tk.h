#ifndef KV_TK_H
#define KV_TK_H

#ifdef USE_TK

#include "kv/base.h"
#include <tkrzw_dbm_hash.h>
#include <tkrzw_dbm_tiny.h>

class KV_TK_DISK_T : public KV_BASE_T {
private:
    tkrzw::HashDBM *db;
public:
    KV_TK_DISK_T(const std::string &, uint64_t = 0);
    bool        close(void) { db->Synchronize(true); return db->Close().IsOK(); }
    void        clear(void) { db->Clear(); }
    uint32_t    count(void);
    uint32_t    add(std::string_view key);
    uint32_t    add(const uint256_t &key)
                { return add(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get(std::string_view key);
    uint32_t    get(const uint256_t &key)
                { return get(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get_or_add(std::string_view key);
};

class KV_TK_INMEM_T : public KV_BASE_T {
private:
    tkrzw::TinyDBM  *db = nullptr;
public:
    KV_TK_INMEM_T(uint64_t = 0);
    bool        close(void) { return db->Close().IsOK(); }
    void        clear(void) { db->Clear(); }
    uint32_t    count(void) { return db->CountSimple(); }
    uint32_t    add(std::string_view key);
    uint32_t    add(const uint256_t &key)
                { return add(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get(std::string_view key);
    uint32_t    get(const uint256_t &key)
                { return get(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get_or_add(std::string_view key);
};

#endif // USE_TK

#endif // KV_TK_H
