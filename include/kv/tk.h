#ifndef KV_TK_H
#define KV_TK_H

#ifdef USE_TK

#include <filesystem>
#include <memory>
#include <tkrzw_dbm_hash.h>
#include <tkrzw_dbm_tiny.h>
#include "kv/base.h"

class KV_TK_DISK_T : public KV_BASE_T {
private:
  uint64_t    tune = 0;
  std::filesystem::path dbpath;
  std::unique_ptr<tkrzw::HashDBM> db = nullptr;
public:
  KV_TK_DISK_T(const std::filesystem::path &, KVNAME_T, uint64_t = 0);
  ~KV_TK_DISK_T();
  bool        open(void);
  bool        close(void);
  void        clear(void) { db->Clear(); }
  uint32_t    count(void);
  uint32_t    add(const std::string_view &key);
  uint32_t    get(const std::string_view &key);
  uint32_t    get_or_add(const std::string_view &key);
};

class KV_TK_INMEM_T : public KV_BASE_T {
private:
  uint64_t    tune;
  std::string dbname;
  std::unique_ptr<tkrzw::TinyDBM> db = nullptr;
public:
  KV_TK_INMEM_T(KVNAME_T name, uint64_t tune = 0) : tune(tune), dbname(kv_name[name]) {}
  bool        open(void);
  bool        close(void);
  void        clear(void) { db->Clear(); }
  uint32_t    count(void) { return db->CountSimple(); }
  uint32_t    add(const std::string_view &key);
  uint32_t    get(const std::string_view &key);
  uint32_t    get_or_add (const std::string_view &key);
};

#endif // USE_TK

#endif // KV_TK_H
