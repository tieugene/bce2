/*
 * Key-value backend.
 * Tkrzw version.
 */
#ifdef USE_TK
#include "kv/tk.h"
#include "misc.h"

using namespace std;

bool        KV_TK_DISK_T::init(const string &s, uint64_t tune) {
  tkrzw::HashDBM::TuningParameters tuning_params;
  tuning_params.offset_width = 5;
  if (tune) {
    if (tune > 30)
      return b_error("Tuning parameter too big: " + to_string(tune));
    else
      tuning_params.num_buckets = 1<<tune;
  }
  if (!db.OpenAdvanced(s + ".tkh", true, tkrzw::File::OPEN_TRUNCATE, tuning_params).IsOK())
    return b_error("Can't open db '" + s + "'.");
  return db.IsOpen();
}

bool    KV_TK_DISK_T::close(void) {
    if (db.IsOpen()) {
        db.Synchronize(true);
        db.Close();
    }
    return (!db.IsOpen());
}

uint32_t    KV_TK_DISK_T::count(void) {
    auto retvalue = db.CountSimple();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_TK_DISK_T::add(string_view key) {
    uint32_t value = count();
    if (value != NOT_FOUND_U32)
        if (!db.Set(key, string_view((const char *)&value, sizeof(uint32_t))).IsOK())
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_TK_DISK_T::get(string_view key) {
    string value;
    if (!db.Get(key, &value).IsOK())    // FXIME: handle errors
        return NOT_FOUND_U32;
    if (value.length() != 4)
      return u32_error("Bad key len: " + to_string(value.length()));
    return *((uint32_t *) value.data());
}

uint32_t    KV_TK_DISK_T::get_or_add(std::string_view key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto status = db.Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (status == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!status.IsOK())
      return u32_error("Something wrong with get_or_add");
  }
  return value;
}

// In-mem

bool        KV_TK_INMEM_T::init(const string &s, uint64_t tune) {
  if (tune) {
    if (tune > 30)
      return b_error("Tuning parameter is too big: " + to_string(tune));
    else
      if (db)
        delete db;
      db = new tkrzw::TinyDBM(1<<tune);
  };
  return (db->IsOpen());
}

uint32_t    KV_TK_INMEM_T::count(void) {
    auto retvalue = db->CountSimple();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_TK_INMEM_T::add(string_view key) {
    uint32_t value = count();
    if (value != NOT_FOUND_U32)
        if (!db->Set(key, string_view((const char *)&value, sizeof(uint32_t))).IsOK())
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_TK_INMEM_T::get(string_view key) {
    string value;
    if (!db->Get(key, &value).IsOK())    // FXIME: handle errors
        return NOT_FOUND_U32;
    if (value.length() != 4)
      return u32_error("Bad key len: " + to_string(value.length()));
    return *((uint32_t *) value.data());
}

uint32_t    KV_TK_INMEM_T::get_or_add(std::string_view key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto status = db->Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (status == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!status.IsOK())
      return u32_error("Something wrong with get_or_add");
  }
  return value;
}

#endif