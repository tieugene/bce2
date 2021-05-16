/*
 * Key-value backend.
 * Tkrzw version.
 */
#ifdef USE_TK

#include "kv/tk.h"
#include "misc.h"

using namespace std;

KV_TK_DISK_T::KV_TK_DISK_T(const filesystem::path &s, uint64_t tune) {
  if (!open(s, tune))
    throw BCException("tkf: Cannot init DB");
}

KV_TK_DISK_T::~KV_TK_DISK_T() {
  if (db)
    close();
  delete db;
}

bool        KV_TK_DISK_T::open(const filesystem::path &s, uint64_t tune) {
  auto bnum_need = 0;
  db = new tkrzw::HashDBM();
  tkrzw::HashDBM::TuningParameters tuning_params;
  tuning_params.offset_width = 5;
  if (tune) {
    if (tune > 30)
      return b_error("tkf: Tuning parameter is too big: " + to_string(tune));
    else {
      bnum_need = 1 << tune;
      tuning_params.num_buckets = bnum_need;
    }
  }
  if (!db->OpenAdvanced(s.string() + ".tkh", true, tkrzw::File::OPEN_DEFAULT, tuning_params).IsOK())
    return b_error("tkf: Cannot open db " + s.string());
  if (OPTS.verbose and bnum_need) { // chk tune
    auto bnum_found = db->CountBuckets();
    if (bnum_found < bnum_need)
      cerr << s.string() << " buckets: found " << bnum_found << " < " << bnum_need << " required." << endl;
  }
  return true;
}

bool        KV_TK_DISK_T::close(void) {
  bool retvalue = true;
  if (!db->Synchronize(true).IsOK())
    retvalue = b_error("tkf: Cannot sync DB");
  if (!db->Close().IsOK())
    retvalue = b_error("tkf: Cannot close DB");
  return retvalue;
}

uint32_t    KV_TK_DISK_T::count(void) {
    auto retvalue = db->CountSimple();
    return (retvalue < 0) ? NOT_FOUND_U32 : uint32_t(retvalue);
}

uint32_t    KV_TK_DISK_T::add(string_view key) {
    uint32_t value = count();
    if (value != NOT_FOUND_U32)
        if (!db->Set(key, string_view((const char *)&value, sizeof(uint32_t))).IsOK())
            value = NOT_FOUND_U32;
    return value;
}

uint32_t    KV_TK_DISK_T::get(string_view key) {
    string value;
    if (!db->Get(key, &value).IsOK())    // FXIME: handle errors
        return NOT_FOUND_U32;
    if (value.length() != 4)
      return u32_error("Bad key len: " + to_string(value.length()));
    return *((uint32_t *) value.data());
}

uint32_t    KV_TK_DISK_T::get_or_add(std::string_view key) {
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

/// In-mem
KV_TK_INMEM_T::KV_TK_INMEM_T(uint64_t tune) {
  if (!open(tune))
    throw BCException("tkm: Cannot init DB.");
}

bool    KV_TK_INMEM_T::open(uint64_t tune) {
  if (tune) {
    if (tune > 30)
      return b_error("tkm: Tuning parameter is too big: " + to_string(tune));
    else
      db = new tkrzw::TinyDBM(1<<tune);
  } else
    db = new tkrzw::TinyDBM();
  return (db);
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
