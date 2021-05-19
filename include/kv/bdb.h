#ifndef KV_BDB_T_H
#define KV_BDB_T_H

#ifdef USE_BDB

#include <filesystem>
#include <db_cxx.h>
#include "kv/base.h"

class KV_BDB_T : public KV_BASE_T {
  // TODO: Hash table size
private:
  std::filesystem::path dbpath;
  Db       *db = nullptr;
  uint32_t  reccount = NOT_FOUND_U32;
  bool      open(const std::filesystem::path &, KVNAME_T, uint64_t);
  bool      close(void);
public:
  KV_BDB_T(const std::filesystem::path &, KVNAME_T, uint64_t = 0);
  ~KV_BDB_T();
  void      clear(void);
  uint32_t  count(void);
  uint32_t  add(std::string_view key);
  uint32_t  get(std::string_view key);
  uint32_t  get_or_add(std::string_view key);
};

#endif // USE _BDB

#endif // KV_BDB_T_H
