#ifndef KV_TK_H
#define KV_TK_H

#ifdef USE_TK

#include <filesystem>
#include <tkrzw_dbm_hash.h>
#include <tkrzw_dbm_tiny.h>
#include "kv/base.h"

class KV_TK_DISK_T : public KV_BASE_T {
private:
  std::string dbpath;
  tkrzw::HashDBM *db;
  bool        open(const std::filesystem::path &, uint64_t = 0);
  bool        close(void);
public:
  KV_TK_DISK_T(const std::filesystem::path &, uint64_t = 0);
  ~KV_TK_DISK_T();
  void        clear(void) { db->Clear(); }
  uint32_t    count(void);
  uint32_t    add(std::string_view key);
  uint32_t    get(std::string_view key);
  uint32_t    get_or_add(std::string_view key);
};

class KV_TK_INMEM_T : public KV_BASE_T {
private:
  std::string dbname;
  tkrzw::TinyDBM  *db = nullptr;
  bool        open(const char *, uint64_t = 0);
public:
  KV_TK_INMEM_T(const char *, uint64_t = 0);
  ~KV_TK_INMEM_T() { delete db; }
  void        clear(void) { db->Clear(); }
  uint32_t    count(void) { return db->CountSimple(); }
  uint32_t    add(std::string_view key);
  uint32_t    get(std::string_view key);
  uint32_t    get_or_add(std::string_view key);
};

#endif // USE_TK

#endif // KV_TK_H
