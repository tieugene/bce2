#include "bk/bk.h"
#include "bce.h"      // STAT, OPTS
//#include "uintxxx.h"  // hash

using namespace std;

// == VOUT ==
VOUT_T::VOUT_T(UNIPTR_T &uptr, const uint32_t no, const uint32_t tx_no, const uint32_t bk_no)
  : no(no), tx_no(tx_no), bk_no(bk_no) {
  satoshi = uptr.take_64();
  auto s_size = uptr.take_varuint();
  auto s_body = uptr.take_ch_ptr(s_size);
  script = string_view(s_body, s_size);
  // cerr << "+VO " << to_string(no) << endl;
}

VOUT_T::~VOUT_T() {
  delete addr;
  // cerr << "-VO " << to_string(no) << endl;
}

bool VOUT_T::parse(void) {
  // TODO: create address
  try {
    addr = addr_decode(script);
  } catch (const AddrException &e) {
    if (OPTS.verbose > DBG_MIN)  // FIXME: DBG_MAX
      cerr << "Vout " << to_string(bk_no) << "/" << to_string(tx_no) << "/" << to_string(no) << ": " << e.what() << endl;
  }
  if (addr)
    STAT.addrs++;
  return true;
}

bool VOUT_T::resolve(void) {
  if (addr) {
    auto k = addr->as_key();
    if (!k.empty())
      addr_id = AddrDB->get(k);
  }
  return true;  // FIXME: addr_id != NOT_FOUND
}

const string VOUT_T::addr_type(void) {
  if (addr)
    return addr->name();
  else
    return "nonstandard";
}

const string VOUT_T::addr_repr(void) {
  if (addr)
    return addr->repr();
  else
    return string();
}
