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

uint32_t    KV_T::add(string_view key) {
    uint32_t value = count();
    if (value != NOT_FOUND_U32)
        if (!db.Set(key, string_view((const char *)&value, sizeof(uint32_t))).IsOK())
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_T::get(string_view key) {
    string value;
    if (!db.Get(key, &value).IsOK())    // FXIME: handle errors
        return NOT_FOUND_U32;
    return *((uint32_t *) value.data());
}

uint32_t    KV_T::get_or_add(std::string_view key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto status = db.Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (status == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!status.IsOK()) {
      cerr << "Something wrong with get_or_add" << endl;
      value = NOT_FOUND_U32;
    }
  }
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
