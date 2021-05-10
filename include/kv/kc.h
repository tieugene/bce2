#ifndef KV_KC_H
#define KV_KC_H

#include "kv/base.h"
#include <kchashdb.h>
#include <kcstashdb.h>

class KV_KC_DISK_T : public KV_BASE_T {
protected:
    kyotocabinet::HashDB *db;
public:
    KV_KC_DISK_T(const std::string &, uint64_t = 0);
    bool        close(void) { db->synchronize(); return db->close(); }
    void        clear(void) { db->clear(); }
    uint32_t    count(void);
    uint32_t    add(std::string_view key);
    uint32_t    add(const uint256_t &key)
                { return add(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get(std::string_view key);
    uint32_t    get(const uint256_t &key)
                { return get(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get_or_add(std::string_view key);
};

class KV_KC_INMEM_T : public KV_BASE_T {
protected:
    kyotocabinet::StashDB *db = nullptr;
public:
    KV_KC_INMEM_T(u_int64_t = 0);
    bool        close(void) { return db->close(); }
    void        clear(void) { db->clear(); }
    uint32_t    count(void) { return db->count(); }
    uint32_t    add(std::string_view key);
    uint32_t    add(const uint256_t &key)
                { return add(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get(std::string_view key);
    uint32_t    get(const uint256_t &key)
                { return get(std::string_view(reinterpret_cast<const char *>(std::data(key)), sizeof(uint256_t))); }
    uint32_t    get_or_add(std::string_view key);
};

#endif // KV_KC_H
