/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <kcpolydb.h>
//include <array>
//include <unordered_map>
#include <stdio.h>
#include "uintxxx.h"

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

// == common ==
class KV_T {
protected:
    virtual uint32_t    real_add(const uint8_t *, const uint16_t) = 0;
    virtual uint32_t    real_get(const uint8_t *, const uint16_t) = 0;
public:
    virtual void        clear(void) = 0;
    virtual uint32_t    count(void) = 0;
    virtual bool        cpto(KV_T *, bool) = 0;
    virtual bool        add(const string&, const uint32_t) = 0;
    uint32_t    add(const uint256_t &key)
                { return real_add(key.begin(), sizeof(uint256_t)); }
    uint32_t    add(const uint160_t &key)
                { return real_add(key.begin(), sizeof(uint160_t)); }
    uint32_t    add(const uint160_t key[], const uint8_t len)
                { return real_add(key[0].begin(), sizeof(uint160_t) * len); }
    uint32_t    get(const uint256_t &key)
                { return real_get(key.begin(), sizeof(uint256_t)); }
    uint32_t    get(const uint160_t &key)
                { return real_get(key.begin(), sizeof(uint160_t)); }
    uint32_t    get(const uint160_t key[], const uint8_t len)
                { return real_get(key[0].begin(), sizeof(uint160_t) * len); }
};

// == kyotocabinet ==
// 150k = 117" (2337716 addrs)
class   KVKC_T : public KV_T {
private:
  kyotocabinet::PolyDB     db;
  uint32_t      real_add(const uint8_t *, const uint16_t);
  uint32_t      real_get(const uint8_t *, const uint16_t);
public:
  // virt replacings
  void          clear(void) { db.clear(); }
  uint32_t      count(void);
  bool          cpto(KV_T *, bool = false);
  bool          add(const string&, const uint32_t);
  // spec
  bool          init(const string &);
};

// == inmem ==
// 150k = 12" (combo +17") 2337716 addrs)
class   KVMEM_T : public KV_T {
private:
    unordered_map <string, uint32_t> db;
    uint32_t      real_add(const uint8_t *, const uint16_t);
    uint32_t      real_get(const uint8_t *, const uint16_t);
public:
    void          clear(void) { db.clear(); }
    uint32_t      count(void) { return db.size(); }
    bool          cpto(KV_T *, bool = false);
    bool          add(const string&, const uint32_t);
};

#endif // KV_H

