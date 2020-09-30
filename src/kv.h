/** Key-value storage */
#ifndef KV_H
#define KV_H

#include "uintxxx.h"
#include <kcpolydb.h>
#include <array>
#include <unordered_map>

//#define MEM

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

class KV_T {
protected:
    virtual uint32_t  real_add(const uint8_t *, const size_t) = 0;
    virtual uint32_t  real_get(const uint8_t *, const size_t) = 0;
public:
    // KV_T(void) {}
    virtual bool        init(const string &) = 0;
    virtual void        clear(void) = 0;
    virtual uint32_t    count(void) = 0;
    uint32_t    add(const uint256_t &key)
                { return real_add(key.begin(), sizeof(uint256_t)); }
    uint32_t    add(const uint160_t &key)
                { return real_add(key.begin(), sizeof(uint160_t)); }
    uint32_t    add(const uint160_t key[], const size_t len)
                { return real_add(key[0].begin(), sizeof(uint160_t) * len); }
    uint32_t    get(const uint256_t &key)
                { return real_get(key.begin(), sizeof(uint256_t)); }
    uint32_t    get(const uint160_t &key)
                { return real_get(key.begin(), sizeof(uint160_t)); }
    uint32_t    get(const uint160_t key[], const size_t len)
                { return real_get(key[0].begin(), sizeof(uint160_t) * len); }
};

// kyotocabinet
class   KVDB_T : public KV_T {
private:
  kyotocabinet::PolyDB     db;
  uint32_t      real_add(const uint8_t *, const size_t);
  uint32_t      real_get(const uint8_t *, const size_t);
public:
  // KVDB_T(void) {}
  bool          init(const string &);
  void          clear(void) { db.clear(); }
  uint32_t      count(void);
};

// inmemory
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
