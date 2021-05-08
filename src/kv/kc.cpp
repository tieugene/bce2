/*
 * Key-value backend.
 * Kyotocabinet version.
 */
#ifndef TKRZW
#include "kv.h"
#include "misc.h"

bool  KV_T::init(const string &s) {

  opened = db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE);
  if (!opened)
    return false;
  //if (!db.tune_buckets(1<<30))  // must be _before_ creating DB
    // throw BCException "Can't open db '"; // + s + "'";
  return opened;
}

bool    KV_T::close(void) {
    if (opened) {
        db.synchronize();
        opened = !db.close();
    }
    return (!opened);
}

void  KV_T::clear(void) {
  db.clear();
}

uint32_t    KV_T::count(void) {
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_T::add(string_view key) {
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add(key.data(), key.length(), (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_T::get(string_view key) {
    uint32_t value;
    auto result = db.get(key.data(), key.length(), (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_T::get_or_add(std::string_view key) {
  auto v = get(key);
  if (v == NOT_FOUND_U32) {
    v = add(key);
    if (v == NOT_FOUND_U32)
      cerr << "Cannot add key '" << ptr2hex(key) << endl;
  }
  return v;
}

bool        KV_T::cpto(KV_T *dst)
{
    /*
        auto cur = db.cursor();
        cur->jump();
        string key, cvalue;
        while (cur->get(&key, &cvalue, true))    // string:string
            dst->add(key, *((uint32_t *) cvalue.c_str()));
        delete cur;
    */
    kyotocabinet::BasicDB *tmp[1];
    tmp[0] = &db;
    return dst->db.merge(tmp, 1, kyotocabinet::PolyDB::MADD);
}
#endif
