/*
 * Key-value backend.
 * Tkrzw version.
 */
#ifdef USE_TK

#include "bce.h"

using namespace std;

KV_TK_DISK_T::KV_TK_DISK_T(const filesystem::path &dir, KVNAME_T name, uint64_t tune) {
  if (!open(dir, name, tune))
    throw BCException("tkf: Cannot init DB");
}

KV_TK_DISK_T::~KV_TK_DISK_T() {
  if (db)
    close();
  delete db;
}

bool KV_TK_DISK_T::open(const filesystem::path &dir, KVNAME_T name, uint64_t tune) {
  dbpath = dir / (kv_name[name] + ".tkh");
  auto bnum_need = 0;
  db = new tkrzw::HashDBM();
  tkrzw::HashDBM::TuningParameters tuning_params;
  tuning_params.offset_width = 5;
  if (tune) {
    if (tune > 30)
      return b_error("tkf " + dbpath.string() + ": Tuning parameter is too big: " + to_string(tune));
    else {
      bnum_need = 1 << tune;
      tuning_params.num_buckets = bnum_need;
    }
  }
  auto s = db->OpenAdvanced(dbpath, true, tkrzw::File::OPEN_DEFAULT, tuning_params);
  if (!s.IsOK())
    return b_error("tkf " + dbpath.string() + ": Cannot open DB - " + s.GetMessage());
  if (OPTS.verbose and bnum_need) { // chk tune
    auto bnum_found = db->CountBuckets();
    if (bnum_found < bnum_need)
      v_error("tkf " + dbpath.string() + " buckets: found " + to_string(bnum_found) + " < " + to_string(bnum_need) + " required.");
  }
  if (!db->IsHealthy()) {
    db->Close();
    delete db;
    return b_error("tkf " + dbpath.string() + ": DB is not healthy.");
  }
  return true;
}

bool KV_TK_DISK_T::close(void) {
  bool retvalue = true;
  auto s = db->Synchronize(true);
  if (!s.IsOK())
    retvalue = b_error("tkf " + dbpath.string() + ": Cannot sync DB - " + s.GetMessage());
  s = db->Close();
  if (!s.IsOK())
    retvalue = b_error("tkf " + dbpath.string() + ": Cannot close DB - " + s.GetMessage());
  return retvalue;
}

uint32_t    KV_TK_DISK_T::count(void) {
  int64_t counter;
  auto s = db->Count(&counter);
  if (s.IsOK())
    return counter; // FIXME: chk > uin32
  return u32_error("tkf " + dbpath.string() + ": Cannot count DB - " + s.GetMessage());
}

uint32_t    KV_TK_DISK_T::add(const string_view &key) {
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(value)));
    if (!s.IsOK())
      value = u32_error("tkf " + dbpath.string() + ": Cannot add to DB - " + s.GetMessage());
  }
  return value;
}

uint32_t    KV_TK_DISK_T::get(const string_view &key) {
    string value;
    auto s = db->Get(key, &value);
    if (s == tkrzw::Status::NOT_FOUND_ERROR)
      return NOT_FOUND_U32;
    else if (s.IsOK()) {   // FIXME: handle errors
      if (value.length() == 4)
        return *((uint32_t *) value.data());
      else
        throw BCException("tkf " + dbpath.string() + ": Bad key len: " + to_string(value.length()));
    } else
      throw BCException("tkf " + dbpath.string() + ": Cannot get from DB - " + s.GetMessage());
}

uint32_t    KV_TK_DISK_T::get_or_add(const std::string_view &key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (s == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!s.IsOK())
      return u32_error("tkf " + dbpath.string() + ": Can not get nor add record - " + s.GetMessage());
  }
  return value;
}

/// In-mem
KV_TK_INMEM_T::KV_TK_INMEM_T(KVNAME_T name, uint64_t tune) {
  if (!open(name, tune))
    throw BCException("tkm " + dbname + ": Cannot init DB.");
}

bool    KV_TK_INMEM_T::open(KVNAME_T name, uint64_t tune) {
  dbname = kv_name[name];
  if (tune) {
    if (tune > 30)
      return b_error("tkm " + dbname + ": Tuning parameter is too big: " + to_string(tune));
    else
      db = new tkrzw::TinyDBM(1<<tune);
  } else
    db = new tkrzw::TinyDBM();
  return (db);
}

uint32_t    KV_TK_INMEM_T::add(const string_view &key) {
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(value)));
    if (!s.IsOK())
      value = u32_error("tkm " + dbname + ": Cannot add to DB - " + s.GetMessage());
  }
  return value;
}

uint32_t    KV_TK_INMEM_T::get(const string_view &key) {
  string value;
  auto s = db->Get(key, &value);
  if (s == tkrzw::Status::NOT_FOUND_ERROR)
    return NOT_FOUND_U32;
  else if (s.IsOK()) {   // FIXME: handle errors
    if (value.length() == 4)
      return *((uint32_t *) value.data());
    else
      throw BCException("tkm " + dbname + ": Bad key len: " + to_string(value.length()));
  } else
    throw BCException("tkm " + dbname + ": Cannot get from DB - " + s.GetMessage());
}

uint32_t    KV_TK_INMEM_T::get_or_add(const std::string_view &key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (s == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!s.IsOK())
      return u32_error("tkm " + dbname + ": Can not get nor add record - " + s.GetMessage());
  }
  return value;
}

#endif
