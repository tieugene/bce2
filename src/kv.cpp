#include "kv.h"

bool        KVDB_T::init(const string &s)
{
    return db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE); // | kyotocabinet::PolyDB::OTRUNCATE); // TODO:
}

uint32_t    KVDB_T::count(void)
{
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KVDB_T::real_add(const uint8_t *key, const size_t size)
{
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add((const char *)key, size, (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KVDB_T::real_get(const uint8_t *key, const size_t size)
{
    uint32_t value;
    auto result = db.get((const char *)key, size, (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

