/*
 * Key-value backend.
 * Tkrzw version.
 */
#ifdef TKRZW
#include "kv.h"
#include "misc.h"

bool        KV_T::init(const string &s)
{
    if (db.Open(s, true, tkrzw::File::OPEN_TRUNCATE) != tkrzw::Status::SUCCESS)
        cerr << "Can't open db '" << s << "'." << endl;
    return db.IsOpen();
}

bool    KV_T::close(void)
{
    if (db.IsOpen()) {
        db.Synchronize(true);
        db.Close();
    }
    return (!db.IsOpen());
}

void  KV_T::clear(void) {
  db.Clear();
}

uint32_t    KV_T::count(void)
{
    auto retvalue = db.CountSimple();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_T::add_raw(const uint8_t *key, const uint16_t size)
{
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = count();
    if (value != NOT_FOUND_U32)
        if (db.Set(string_view((const char *) key, size), string_view((const char *)&value, sizeof(uint32_t))) != tkrzw::Status::SUCCESS)
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_T::get_raw(const uint8_t *key, const uint16_t size)
{
    uint32_t value;
    if (db.Get(string_view((const char *)key, size), (string *)&value) != tkrzw::Status::SUCCESS)
        value = NOT_FOUND_U32;
    return value;
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
    return db.Export(&dst->db) == tkrzw::Status::SUCCESS;
}
#endif
