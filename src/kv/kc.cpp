/*
 * Key-value backend.
 * Kyotocabinet version.
 */
#include "kv/kc.h"
#include "misc.h"

using namespace std;

KV_KC_DISK_T::KV_KC_DISK_T(const string &s, uint64_t tune) {
  db = new kyotocabinet::HashDB();
  if (tune) {
    if (tune > 30)
      throw BCException("kcf: Tuning parameter is too big: " + to_string(tune));
    else
      if (!db->tune_buckets(1<<tune))  // must be _before_ creating DB
        throw BCException("kcf: Cannot tune");
  }
  if (!db->open(s + ".kch", kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE))
    throw BCException("kcf: Cannot open DB");
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
      cerr << "Cannot add key '" << ptr2hex(key) << endl;
  }
  return v;
}

/// StashDB

KV_KC_INMEM_T::KV_KC_INMEM_T(u_int64_t tune) {
  db = new kyotocabinet::StashDB();
  if (tune) {
    if (tune > 30)
      throw BCException("kcm: Tuning parameter is too big: " + to_string(tune));
    else
      if (!db->tune_buckets(1<<tune))
        throw BCException("kcm: Cannot tune");
  }
  if (!db->open(":"))
    throw BCException("kcm: Cannot open DB");
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
      cerr << "Cannot add key '" << ptr2hex(key) << endl;
  }
  return v;
}
