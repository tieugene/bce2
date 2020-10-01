/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <kcpolydb.h>
//include <array>
//include <unordered_map>
#include <stdio.h>
#include "uintxxx.h"

//#define MEM

const uint32_t NOT_FOUND_U32 = 0xFFFFFFFF;

using namespace std;

class KV_T {
protected:
    virtual uint32_t    real_add(const uint8_t *, const uint16_t) = 0;
    virtual uint32_t    real_get(const uint8_t *, const uint16_t) = 0;
public:
    virtual bool        init(const string &) = 0;
    virtual void        clear(void) = 0;
    virtual uint32_t    count(void) = 0;
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

// kyotocabinet
class   KVDB_T : public KV_T {
private:
  kyotocabinet::PolyDB     db;
  uint32_t      real_add(const uint8_t *, const uint16_t);
  uint32_t      real_get(const uint8_t *, const uint16_t);
public:
  bool          init(const string &);
  void          clear(void) { db.clear(); }
  uint32_t      count(void);
};
/*
// inmemory
struct VARRAY_T {
    uint16_t    size;
    uint8_t     data[];
   //  bool operator==(const VARRAY_T &alien) const
   //     { return (size == alien.size && memcmp(data, alien.data, size) == 0);}
};
//bool operator==(const VARRAY_T& one, const VARRAY_T& two)
//    { return ((one.size == two.size) && (memcmp(one.data, two.data, one.size) == 0));}

// Extend std and boost namespaces with our hash wrappers.
//-----------------------------------------------------------------------------
// get from libbitcoin-system/include/bitcoin/system/math/hash.hpp

namespace std
{
    template<>
    struct hash<VARRAY_T>
    {
        size_t operator()(const VARRAY_T& hash) const
        {
            return boost::hash_range(hash.data, hash.data + hash.size);
        }
    };
} // namespace std

namespace boost
{
template<>
struct hash<VARRAY_T>
{
    size_t operator()(const VARRAY_T& hash) const
    {
        return boost::hash_range(hash.data, hash.data + hash.size);
    }
};
} // namespace boost


struct AvHash {
    size_t operator()(const VARRAY_T& k) const
    { return boost::hash_range(k.data, k.data + k.size); }
};
auto MyHash = [](const VARRAY_T& k)
    { return boost::hash_range(k.data, k.data + k.size); };
auto AvEq = [](const VARRAY_T& one, const VARRAY_T& two)
    { return ((one.size == two.size) && (memcmp(one.data, two.data, one.size) == 0));};

class   KVMEM_T : public KV_T {
private:
    unordered_map <VARRAY_T, uint32_t, decltype(MyHash), decltype(AvEq)> db; // FIXME: hash, equal funcs
    uint32_t      real_add(const uint8_t *, const uint16_t);
    uint32_t      real_get(const uint8_t *, const uint16_t);
public:
    bool          init(const string &) { return true; }
    void          clear(void) { db.clear(); }
    uint32_t      count(void) { return db.size(); }
};
*/
#endif // KV_H

