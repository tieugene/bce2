/*
 * Key-value backend.
 * Tkrzw version.
 */
#ifdef USE_TK

#include "bce.h"

using namespace std;

KV_TK_DISK_T::KV_TK_DISK_T(const std::filesystem::path &dir, KVNAME_T name, uint64_t tune) : tune(tune) {
  dbpath = dir / (kv_name[name] + ".tkh");
  db = make_unique<tkrzw::HashDBM>();
}

KV_TK_DISK_T::~KV_TK_DISK_T() {
  if (db->IsOpen())
    db->Close();
}

const std::string KV_TK_DISK_T::err_prefix(void) {
  return "tkf " + dbpath.string() + ": ";
}

bool KV_TK_DISK_T::open(void) {
  auto bnum_need = 0;
  tkrzw::HashDBM::TuningParameters tuning_params;
  tuning_params.offset_width = 5;
  if (tune) {
    if (tune > 30)
      return b_error(err_prefix() + "Tuning parameter is too big: " + to_string(tune));
    else {
      bnum_need = 1 << tune;
      tuning_params.num_buckets = bnum_need;
    }
  }
  auto s = db->OpenAdvanced(dbpath, true, tkrzw::File::OPEN_DEFAULT, tuning_params);
  if (!s.IsOK())
    return b_error(err_prefix() + "Cannot open DB - " + s.GetMessage());
  if (OPTS.verbose and bnum_need) { // chk tune
    auto bnum_found = db->CountBuckets();
    if (bnum_found < bnum_need)
      v_error(err_prefix() + "buckets: found " + to_string(bnum_found) + " < " + to_string(bnum_need) + " required.");
  }
  if (!db->IsHealthy()) {
    db->Close();
    return b_error(err_prefix() + "DB is not healthy.");
  }
  return true;
}

bool KV_TK_DISK_T::close(void) {
  bool retvalue = true;
  if (db->IsOpen()) {
    auto s = db->Synchronize(true);
    if (!s.IsOK())
      retvalue = b_error(err_prefix() + "Cannot sync DB - " + s.GetMessage());
    s = db->Close();
    if (!s.IsOK())
      retvalue = b_error(err_prefix() + "Cannot close DB - " + s.GetMessage());
  }
  return retvalue;
}

uint32_t    KV_TK_DISK_T::count(void) {
  int64_t counter;
  auto s = db->Count(&counter);
  if (s.IsOK())
    return counter; // FIXME: chk > uin32
  return u32_error(err_prefix() + "Cannot count DB - " + s.GetMessage());
}

uint32_t    KV_TK_DISK_T::add(const string_view &key) {
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(value)), false);
    if (!s.IsOK())
      value = u32_error(err_prefix() + "Cannot add to DB - " + s.GetMessage());
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
        throw BCException(err_prefix() + "Bad key len: " + to_string(value.length()));
    } else
      throw BCException(err_prefix() + "Cannot get from DB - " + s.GetMessage());
}

uint32_t    KV_TK_DISK_T::get_or_add(const std::string_view &key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (s == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!s.IsOK())
      return u32_error(err_prefix() + "Can not get nor add record - " + s.GetMessage());
  }
  return value;
}

bool        KV_TK_DISK_T::del(const std::string_view &key) {
  auto s = db->Remove(key);
  if (!s.IsOK())
    return b_error(err_prefix() + "Cannot delete record.");
  return true;
}

/// In-mem
const std::string KV_TK_INMEM_T::err_prefix(void) {
  return "tkm " + dbname + ": ";
}

bool    KV_TK_INMEM_T::open(void) {
  if (tune) {
    if (tune > 30)
      return b_error(err_prefix() + "Tuning parameter is too big: " + to_string(tune));
    else
      db = make_unique<tkrzw::TinyDBM>(1<<tune);
  } else
    db = make_unique<tkrzw::TinyDBM>();
  return bool(db);
}


bool    KV_TK_INMEM_T::close(void) {
  if (db and db->IsOpen())
    db->Close();
  return true;
}

uint32_t    KV_TK_INMEM_T::add(const string_view &key) {
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(value)), false);
    if (!s.IsOK())
      value = u32_error(err_prefix() + "Cannot add to DB - " + s.GetMessage());
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
      throw BCException(err_prefix() + "Bad key len: " + to_string(value.length()));
  } else
    throw BCException(err_prefix() + "Cannot get from DB - " + s.GetMessage());
}

uint32_t    KV_TK_INMEM_T::get_or_add(const std::string_view &key) {
  string old_value;
  uint32_t value = count();
  if (value != NOT_FOUND_U32) {
    auto s = db->Set(key, string_view((const char *)&value, sizeof(uint32_t)), false, &old_value);
    if (s == tkrzw::Status::DUPLICATION_ERROR)
      value = *((uint32_t *) old_value.data());
    else if (!s.IsOK())
      return u32_error(err_prefix() + "Can not get nor add record - " + s.GetMessage());
  }
  return value;
}

bool        KV_TK_INMEM_T::del(const std::string_view &key) {
  auto s = db->Remove(key);
  if (!s.IsOK())
    return b_error(err_prefix() + "Cannot delete record.");
  return true;
}

#endif
