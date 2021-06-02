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
  const std::string err_prefix(void);
public:
  KV_TK_DISK_T(const std::filesystem::path &, KVNAME_T, uint64_t = 0);
  ~KV_TK_DISK_T();
  inline bool infile(void) { return true; }
  bool        open(void);
  bool        close(void);
  void        clear(void) { db->Clear(); }
  uint32_t    count(void);
  uint32_t    add(const std::string_view &);
  uint32_t    get(const std::string_view &);
  uint32_t    get_or_add(const std::string_view &);
  bool        del(const std::string_view &);
};

class KV_TK_INMEM_T : public KV_BASE_T {
private:
  uint64_t    tune;
  std::string dbname;
  std::unique_ptr<tkrzw::TinyDBM> db = nullptr;
  const std::string err_prefix(void);
public:
  KV_TK_INMEM_T(KVNAME_T name, uint64_t tune = 0) : tune(tune), dbname(kv_name[name]) {}
  inline bool infile(void) { return false; }
  bool        open(void);
  bool        close(void);
  void        clear(void) { db->Clear(); }
  uint32_t    count(void) { return db->CountSimple(); }
  uint32_t    add(const std::string_view &);
  uint32_t    get(const std::string_view &);
  uint32_t    get_or_add (const std::string_view &);
  bool        del(const std::string_view &);
};

#endif // USE_TK

#endif // KV_TK_H
