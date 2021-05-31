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

const string VOUT_T::addr_type(void) {
  return (addr) ? addr->name() : "nonstandard";
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
    auto key = addr->as_key();
    retvalue = ((addr_id = AddrDB->get_or_add(key)) != MAX_UINT32);
    if (retvalue) {
      if (addr_id >= COUNT.addr) {  // new
        addr_is_new = true;
        if (addr_id != COUNT.addr) {
          retvalue = b_error("Vout # " + to_string(no) + ": new addr has # " + to_string(addr_id) + " instead of expecting " + to_string(COUNT.addr));
          rollback();
        } else {
          COUNT.addr++;
          STAT.addr_lens[key.length()]++;
        }
      }
    } else
      retvalue = b_error("Vout # " + to_string(no) + " not found nor added.");
  }
  if (!retvalue)
    v_error("Vout # " + to_string(no) + " resolve error");
  return retvalue;
}

bool VOUT_T::rollback(void) { // FIXME: tune counter
  bool retvalue(true);
  if ((addr_id != MAX_UINT32) and (addr_is_new)) {
    if (OPTS.verbose == DBG_MAX)
      v_error("Vout # " + to_string(no) + ": rolling back...");
    retvalue = AddrDB->del(addr->as_key());
    if (retvalue)
      addr_id = MAX_UINT32;
    else
      v_error("Vout # " + to_string(no) + ": Rolling back oops: " + addr->repr());
  }
  return retvalue;
}
