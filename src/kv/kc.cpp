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

bool        KV_KC_DISK_T::open(const filesystem::path &s, uint64_t tune) {
  long bnum_need = 0;
  db = new kyotocabinet::HashDB();
  if (tune) {
    if (tune > 30)
      return b_error("kcf: Tuning parameter is too big: " + to_string(tune));
    else {
      bnum_need = 1<<tune;
      if (!db->tune_buckets(bnum_need))  // must be _before_ creating DB
        return b_error("kcf: Cannot tune");
      }
  }
  if (!db->open(s.string() + ".kch", kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE))
    return b_error("kcf: Cannot open DB " + s.string());
  if (OPTS.verbose and bnum_need) { // chk tune
    map<string, string> status;
    if (db->status(&status)) {
      auto it = status.find("bnum");
      if (it != status.end()) {
        auto bnum_found = atol(it->second.c_str());
        if (bnum_found < bnum_need)
          cerr << s.string() << " buckets: found " << bnum_found << " < " << bnum_need << " required." << endl;
      }
    }
  }
  return true;
}

bool        KV_KC_DISK_T::close(void) {
  bool retvalue = true;
  if (!db->synchronize())
    retvalue = b_error("Cannot sync DB");
  if (!db->close())
    retvalue = b_error("Cannot close DB");
  return retvalue;
}

uint32_t    KV_KC_DISK_T::count(void) {
    auto retvalue = db->count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_KC_DISK_T::add(string_view key) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db->add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_DISK_T::get(string_view key) {
    uint32_t value;
    auto result = db->get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_DISK_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      return u32_error("Cannot add key '" + ptr2hex(key));
  }
  return v;
}

/// StashDB
KV_KC_INMEM_T::KV_KC_INMEM_T(u_int64_t tune) {
  if (!open(tune))
    throw BCException("kcm: Cannot init DB.");
}

bool        KV_KC_INMEM_T::open(u_int64_t tune) {
  db = new kyotocabinet::StashDB();
  if (tune) {
    if (tune > 30)
      return b_error("kcm: Tuning parameter is too big: " + to_string(tune));
    else
      if (!db->tune_buckets(1<<tune))
        return b_error("kcm: Cannot tune");
  }
  if (!db->open(":"))
    return b_error("kcm: Cannot open DB");
  return true;
}

uint32_t    KV_KC_INMEM_T::add(string_view key) {
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
      return u32_error("Cannot add key " + ptr2hex(key));
  }
  return v;
}

#endif
