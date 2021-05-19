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
KV_KC_DISK_T::KV_KC_DISK_T(const filesystem::path &s, KVNAME_T name, uint64_t tune) {
  if (!open(s, name, tune))
    throw BCException("kcf: Cannot init DB");
}

KV_KC_DISK_T::~KV_KC_DISK_T() {
  if (db)
    close();
  delete db;
}

bool KV_KC_DISK_T::open(const filesystem::path &dir, KVNAME_T name, uint64_t tune) {
  dbpath = dir / (kv_name[name] + ".kch");
  long bnum_need = 0;
  db = new kyotocabinet::HashDB();
  if (tune) {
    if (tune > 30)
      return b_error("kcf " + dbpath.string() + ": Tuning parameter is too big: " + to_string(tune));
    else {
      bnum_need = 1<<tune;
      if (!db->tune_buckets(bnum_need))  // must be _before_ creating DB
        return b_error("kcf " + dbpath.string() + ": Cannot tune");
      }
  }
  if (!db->open(dbpath, kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE))
    return b_error("kcf " + dbpath.string() + ": Cannot open DB");
  if (OPTS.verbose and bnum_need) { // chk tune
    map<string, string> status;
    if (db->status(&status)) {
      auto it = status.find("bnum");
      if (it != status.end()) {
        auto bnum_found = atol(it->second.c_str());
        if (bnum_found < bnum_need)
          v_error(dbpath.string() + " buckets: found " + it->second + " < " + to_string(bnum_need) + " required.");
      }
    }
  }
  return true;
}

bool KV_KC_DISK_T::close(void) {
  bool retvalue = true;
  if (!db->synchronize())
    retvalue = b_error("Cannot sync DB " + dbpath.string());
  if (!db->close())
    retvalue = b_error("Cannot close DB " + dbpath.string());
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
      return u32_error(dbpath.string() + ": can not find nor add key " + ptr2hex(key));
  }
  return v;
}

/// StashDB
KV_KC_INMEM_T::KV_KC_INMEM_T(KVNAME_T name, u_int64_t tune) {
  if (!open(name, tune))
    throw BCException("kcm " + dbname + ": Cannot init DB.");
}

bool KV_KC_INMEM_T::open(KVNAME_T name, u_int64_t tune) {
  dbname = kv_name[name];
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
