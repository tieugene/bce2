/*
 * Key-value backend.
 * Kyotocabinet version.
 */
#ifdef USE_KC

#include <cstdlib>
#include "kv/kc.h"
#include "misc.h"

using namespace std;

/// HashDB
KV_KC_DISK_T::KV_KC_DISK_T(const filesystem::path &s, uint64_t tune) {
  if (!open(s, tune))
    throw BCException("kcf: Cannot init DB");
}

KV_KC_DISK_T::~KV_KC_DISK_T() {
  if (db)
    close();
  delete db;
}

bool KV_KC_DISK_T::open(const filesystem::path &s, uint64_t tune) {
  dbpath = s.string() + ".kch";
  long bnum_need = 0;
  db = new kyotocabinet::HashDB();
  if (tune) {
    if (tune > 30)
      return b_error("kcf " + dbpath + ": Tuning parameter is too big: " + to_string(tune));
    else {
      bnum_need = 1<<tune;
      if (!db->tune_buckets(bnum_need))  // must be _before_ creating DB
        return b_error("kcf " + dbpath + ": Cannot tune");
      }
  }
  if (!db->open(dbpath, kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE))
    return b_error("kcf " + dbpath + ": Cannot open DB " + s.string());
  if (OPTS.verbose and bnum_need) { // chk tune
    map<string, string> status;
    if (db->status(&status)) {
      auto it = status.find("bnum");
      if (it != status.end()) {
        auto bnum_found = atol(it->second.c_str());
        if (bnum_found < bnum_need)
          v_error(dbpath + " buckets: found " + it->second + " < " + to_string(bnum_need) + " required.");
      }
    }
  }
  return true;
}

bool KV_KC_DISK_T::close(void) {
  bool retvalue = true;
  if (!db->synchronize())
    retvalue = b_error("Cannot sync DB " + dbpath);
  if (!db->close())
    retvalue = b_error("Cannot close DB " + dbpath);
  return retvalue;
}

uint32_t KV_KC_DISK_T::count(void) {
    auto retvalue = db->count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t KV_KC_DISK_T::add(string_view key) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db->add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t KV_KC_DISK_T::get(string_view key) {
    uint32_t value;
    auto result = db->get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t KV_KC_DISK_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      return u32_error(dbpath + ": can not find nor add key " + ptr2hex(key));
  }
  return v;
}

/// StashDB
KV_KC_INMEM_T::KV_KC_INMEM_T(const char *s, u_int64_t tune) {
  if (!open(s, tune))
    throw BCException("kcm " + dbname + ": Cannot init DB.");
}

bool KV_KC_INMEM_T::open(const char *s, u_int64_t tune) {
  dbname = s;
  db = new kyotocabinet::StashDB();
  if (tune) {
    if (tune > 30)
      return b_error("kcm " + dbname + ": Tuning parameter is too big: " + to_string(tune));
    else
      if (!db->tune_buckets(1<<tune))
        return b_error("kcm " + dbname + ": Cannot tune");
  }
  if (!db->open(":"))
    return b_error("kcm " + dbname + ": Cannot open DB");
  return true;
}

uint32_t KV_KC_INMEM_T::add(string_view key) {
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db->add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_INMEM_T::get(string_view key) {
    uint32_t value;
    auto result = db->get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_INMEM_T::get_or_add(string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      return u32_error(dbname + ": can not find nor add key " + ptr2hex(key));
  }
  return v;
}

#endif
