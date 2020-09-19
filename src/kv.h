/** Key-value storage */
#ifndef KV_H
#define KV_H

#include "uintxxx.h"
#include <kcpolydb.h>
#include <array>    // FIXME:
#include <unordered_map>

#define MEM

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

template <typename T> class   KVDB_T {
protected:
  kyotocabinet::PolyDB     db;
public:
  bool        init(const string &s) {
      return db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE | kyotocabinet::PolyDB::OTRUNCATE); // TODO:
  }
  void        clear(void) { db.clear(); }
  uint32_t    count(void) {
      auto retvalue = db.count();
      return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
  }
  uint32_t    add(const T &key) {
      //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
      auto value = count();
      if (value != NOT_FOUND_U32) {
          if (!db.add(reinterpret_cast<char *>(&key), sizeof(T), reinterpret_cast<char *>(&value), sizeof(uint32_t)))
              value = NOT_FOUND_U32;
      }
      return value;
  }
  uint32_t    get(const T &key) {
      uint32_t value;
      auto result = db.get(reinterpret_cast<char *>(&key), sizeof(T), reinterpret_cast<char *>(&value), sizeof(uint32_t));
      if (result != sizeof(uint32_t))
          value = NOT_FOUND_U32;
      return value;
  }
};

typedef KVDB_T <uint256_t> TxDB_T;
typedef KVDB_T <uint160_t> AddrDB_T;

template <typename T> class KVMAP_T {
    unordered_map <T, uint32_t> db; // FIXME: hash, equal funcs
  public:
    bool        init(const string &) { return true; }
    void        clear(void) { db.clear(); }
    uint32_t    count(void) { return db.size(); }
    uint32_t    get(const T &key) {
      auto value = NOT_FOUND_U32;
      auto search = db.find(key);
      if (search != db.end())
          value = search->second;
      return value;
    }
    uint32_t    add(const T &key) {
      auto value = NOT_FOUND_U32;
      if (db.find(key) == db.end()) {
        value = db.size();
        db.emplace(key, value);
      }
      return value;
    }
};

typedef KVMAP_T <uint256_t> TxMAP_T;
typedef KVMAP_T <uint160_t> AddrMAP_T;

#endif // KV_H
