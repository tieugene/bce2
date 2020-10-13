#include "kv.h"
#include "misc.h"

bool        KV_T::init(const string &s)
{
    auto retvalue = db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE);
    if (not retvalue)
        cerr << "Can't oprn db '" << s << "'." << endl;
    return retvalue;
}

uint32_t    KV_T::count(void)
{
    auto retvalue = db.count();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_T::add_raw(const uint8_t *key, const uint16_t size)
{
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (!db.add((const char *)key, size, (const char *)&value, sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_T::get_raw(const uint8_t *key, const uint16_t size)
{
    uint32_t value;
    auto result = db.get((const char *)key, size, (char *)&value, sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

bool        KV_T::cpto(KV_T *dst, bool erase)
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
