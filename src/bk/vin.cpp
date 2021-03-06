#include "misc.h"  // hash2hex
#include "bk/bk.h"

using namespace std;

// == VIN ==
VIN_T::VIN_T(UNIPTR_T &uptr, const uint32_t no, TX_T * const tx)
    : tx(tx), no(no) {
  tx_hash = uptr.take_256_ptr();
  vout = uptr.take_32();
  auto s_size = uptr.take_varuint();
  auto s_body = uptr.take_ch_ptr(s_size);
  script = string_view(s_body, s_size);
  seq = uptr.take_32();
}
/*
bool VIN_T::parse(void) {
  // TODO: mt: start resolve
  return true;
}
*/
bool VIN_T::resolve(void) {
  if (vout != COINBASE_vout)
    if ((tx_id = TxDB->get(u256string_view(*tx_hash))) == NOT_FOUND_U32)
      return b_error("Vin # " + to_string(no) +  " resolve error: txid " + hash2hex(*tx_hash) + " not found.");
  return true;
}
