/*
 * Key-value backend.
 * Kyotocabinet version.
 */
#include "kv/kc.h"
#include "misc.h"

using namespace std;

bool  KV_KC_T::init(const string &s) {

  opened = db.open(s, kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE);
  if (!opened)
    return false;
  //if (!db.tune_buckets(1<<30))  // must be _before_ creating DB
    // throw BCException "Can't open db '"; // + s + "'";
  return opened;
}

bool    KV_KC_T::close(void) {
    if (opened) {
        db.synchronize();
        opened = !db.close();
    }
    return (!opened);
}

void  KV_KC_T::clear(void) {
  db.clear();
}

uint32_t    KV_KC_T::count(void) {
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_KC_T::add(string_view key) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_T::get(string_view key) {
    uint32_t value;
    auto result = db.get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_KC_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      cerr << "Cannot add key '" << ptr2hex(key) << endl;
  }
  return v;
}
