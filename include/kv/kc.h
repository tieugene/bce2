#ifndef KV_KC_H
#define KV_KC_H

#ifdef USE_KC

#include <filesystem>
#include <kchashdb.h>
#include <kcstashdb.h>
#include "kv/base.h"

class KV_KC_DISK_T : public KV_BASE_T {
private:
  std::filesystem::path dbpath;
  kyotocabinet::HashDB *db;
  bool        open(const std::filesystem::path &, KVNAME_T, uint64_t);
  bool        close(void);
public:
  KV_KC_DISK_T(const std::filesystem::path &, KVNAME_T, uint64_t = 0);
  ~KV_KC_DISK_T();
  void        clear(void) { db->clear(); }
  uint32_t    count(void);
  uint32_t    add(const std::string_view &key);
  uint32_t    get(const std::string_view &key);
  uint32_t    get_or_add(const std::string_view &key);
};

class KV_KC_INMEM_T : public KV_BASE_T {
private:
  std::string dbname;
  kyotocabinet::StashDB *db = nullptr;
  bool        open(KVNAME_T, u_int64_t);
public:
  KV_KC_INMEM_T(KVNAME_T, u_int64_t = 0);
  ~KV_KC_INMEM_T() { delete db; }
  void        clear(void) { db->clear(); }
  uint32_t    count(void) { return db->count(); }
  uint32_t    add(const std::string_view &key);
  uint32_t    get(const std::string_view &key);
  uint32_t    get_or_add(const std::string_view &key);
};

#endif // USE_KC

#endif // KV_KC_H
