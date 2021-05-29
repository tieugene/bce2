#include "bce.h"      // STAT, OPTS
#include "bk/bk.h"

using namespace std;

// == VOUT ==
VOUT_T::VOUT_T(UNIPTR_T &uptr, const uint32_t no, TX_T * const tx)
    : tx(tx), no(no) {
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
  if (script.length()) {
    try {
      addr = addr_decode(script);
    } catch (const AddrException &e) {
      if (OPTS.verbose > DBG_MIN)  // FIXME: DBG_MAX
        cerr << "Vout " << to_string(tx->get_bk()->get_id()) << "/" << to_string(tx->get_no()) << "/" << to_string(no) << ": " << e.what() << endl;
    }
  }
  if (addr)
    STAT.addrs++;
  return true;
}

bool VOUT_T::resolve(void) {
  bool retvalue(true);
  if (addr and addr->is_full()) {
    retvalue = ((addr_id = AddrDB->get_or_add(addr->as_key())) != MAX_UINT32);
    if (retvalue) {
      if (addr_id >= COUNT.addr) {  // new
        if (addr_id != COUNT.addr)
          retvalue = b_error("new addr has # " + to_string(addr_id) + " instead of expecting " + to_string(COUNT.addr));
        else {
          addr_is_new = true;
          COUNT.addr++;
        }
      }
    } else
      retvalue = b_error("Vout # " + to_string(no) + " not found nor added");
  }
  if (!retvalue)
    v_error("Vout # " + to_string(no) + " resolve error");
  return retvalue;
}

const string VOUT_T::addr_type(void) {
  if (addr)
    return addr->name();
  else
    return "nonstandard";
}
/*
const string VOUT_T::addr_repr(void) {
  if (addr and addr->is_full())
    return addr->repr();
  else
    return string();
}*/
