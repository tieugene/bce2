#ifdef USE_BDB

#include "kv/bdb.h"
#include "misc.h"

using namespace std;

KV_BDB_T::KV_BDB_T(const filesystem::path &s, uint64_t tune) {
  if (!open(s, tune))
    throw BCException("bdb " + dbpath + ": Cannot init DB");
}

KV_BDB_T::~KV_BDB_T() {
  if (db)
    close();
  delete db;
}

bool KV_BDB_T::open(const filesystem::path &s, uint64_t tune) {
  dbpath = s;
  if (!db)
    db = new Db(nullptr, 0);
  if (!db)
    return b_error("bdb " + dbpath + ": Cannot open db " + s.string());
  if (tune)
      db->set_cachesize(tune, 0, 0);
  if (db->open(nullptr, (s.string() + ".bdb").c_str(), nullptr, DB_HASH, DB_CREATE, 0644))
    return b_error("bdb " + dbpath + ": Cannot open db " + s.string());
  return true;
}

bool KV_BDB_T::close(void) {
  bool retvalue = true;
  if (db->sync(0))
    retvalue = b_error("bdb " + dbpath + ": Cannot sync DB");
  if (db->close(0))
    retvalue = b_error("bdb " + dbpath + ": Cannot close DB");
  return retvalue;
}

void KV_BDB_T::clear(void) {
  uint32_t countp;
  db->truncate(0, &countp, 0);
  reccount = 0;
}

uint32_t KV_BDB_T::count(void) {
  if (reccount == NOT_FOUND_U32) {
    // see libdb-*.tgz/examples/cxx/BtRecExample.cpp
    // db->stat_print(DB_FAST_STAT);
    DB_HASH_STAT *stat;
    if (!db->stat(nullptr, &stat, 0))
      reccount = stat->hash_ndata; // or hash_nkeys
    else
      v_error(dbpath + ": Cannot get stat()");
    free(stat);
  }
  return reccount;
}

uint32_t    KV_BDB_T::add(string_view key) {
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    Dbt k((void *) key.begin(), key.length()), v((void *) &value, sizeof(uint32_t));
    if (db->put(nullptr, &k, &v, DB_NOOVERWRITE))
      value = NOT_FOUND_U32;
    else
      reccount++;
  }
  return value;
}

uint32_t    KV_BDB_T::get(string_view key) {
  Dbt k((void *) key.begin(), key.length()), val;
  if (!db->get(nullptr, &k, &val, 0))
    return *((uint32_t *) val.get_data());
  else
    return NOT_FOUND_U32;
}

uint32_t    KV_BDB_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      return u32_error(dbpath + ": Can not get nor add key " + ptr2hex(key));
  }
  return v;
}

#endif // USE_BDB
