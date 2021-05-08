/*
 * Key-value backend.
 * Kyotocabinet version.
 */
#include "kv/kc.h"
#include "misc.h"

using namespace std;

bool  KV_KC_HASH_T::init(const string &s) {

  opened = db.open(s + ".kch", kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE);
  if (!opened)
    return false;
  //if (!db.tune_buckets(1<<30))  // must be _before_ creating DB
    // throw BCException "Can't open db '"; // + s + "'";
  return opened;
}

bool    KV_KC_HASH_T::close(void) {
    if (opened) {
        db.synchronize();
        opened = !db.close();
    }
    return (!opened);
}

void  KV_KC_HASH_T::clear(void) {
  db.clear();
}

uint32_t    KV_KC_HASH_T::count(void) {
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_KC_HASH_T::add(string_view key) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_HASH_T::get(string_view key) {
    uint32_t value;
    auto result = db.get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_HASH_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      cerr << "Cannot add key '" << ptr2hex(key) << endl;
  }
  return v;
}

/// Stash
bool  KV_KC_STASH_T::init(const string &s) {

  opened = db.open(":", kyotocabinet::StashDB::OWRITER | kyotocabinet::StashDB::OCREATE);
  if (!opened)
    return false;
  //if (!db.tune_buckets(1<<30))  // must be _before_ creating DB
    // throw BCException "Can't open db '"; // + s + "'";
  return opened;
}

bool    KV_KC_STASH_T::close(void) {
    if (opened) {
        db.synchronize();
        opened = !db.close();
    }
    return (!opened);
}

void  KV_KC_STASH_T::clear(void) {
  db.clear();
}

uint32_t    KV_KC_STASH_T::count(void) {
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_KC_STASH_T::add(string_view key) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_STASH_T::get(string_view key) {
    uint32_t value;
    auto result = db.get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_STASH_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      cerr << "Cannot add key '" << ptr2hex(key) << endl;
  }
  return v;
}
