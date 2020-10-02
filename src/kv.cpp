#include "kv.h"
#include "misc.h"

// == file based ==

bool        KVKC_T::init(const string &s)
{
    return db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE); // | kyotocabinet::PolyDB::OTRUNCATE); // TODO:
}

uint32_t    KVKC_T::count(void)
{
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KVKC_T::real_add(const uint8_t *key, const uint16_t size)
{
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add((const char *)key, size, (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KVKC_T::real_get(const uint8_t *key, const uint16_t size)
{
    uint32_t value;
    auto result = db.get((const char *)key, size, (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

bool        KVKC_T::cpto(KV_T *dst, bool erase)
{
    if (erase)
        dst->clear();
    auto cur = db.cursor();
    cur->jump();
    string key, cvalue;
    while (cur->get(&key, &cvalue, true))    // string:string
        dst->add(key, *((uint32_t *) cvalue.c_str()));
    delete cur;
    return true;
}

bool        KVKC_T::add(const string &key, const uint32_t value)
{
    db.add(key, string((const char *)&value, sizeof(value)));
    return true;
}

// == inmem ==

uint32_t    KVMEM_T::real_add(const uint8_t *raw_key, const uint16_t size)
{
    auto key = string((const char *)raw_key, size);
    auto value = NOT_FOUND_U32;
    if (db.find(key) == db.end()) {
        value = db.size();
        db.emplace(key, value);
    }
    return value;
}

uint32_t    KVMEM_T::real_get(const uint8_t *raw_key, const uint16_t size)
{
    auto key = string((const char *)raw_key, size);
    auto value = NOT_FOUND_U32;
    auto search = db.find(key);
    if (search != db.end())
        value = search->second;
    return value;
}

bool        KVMEM_T::cpto(KV_T *dst, bool erase)
{
    if (erase)
        dst->clear();
    for(const auto& kv : db)
        dst->add(kv.first, kv.second);    // string:uint32_t
    return true;
}

bool        KVMEM_T::add(const string &key, const uint32_t value)
{
    db.emplace(key, value);
    return true;
}
