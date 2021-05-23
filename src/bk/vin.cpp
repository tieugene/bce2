#include "bk/bk.h"
#include "bce.h"      // TxDB
#include "uintxxx.h"  // hash2hex

using namespace std;

// == VIN ==
VIN_T::VIN_T(UNIPTR_T &uptr, const uint32_t vin_no) : no(vin_no) {
  tx_hash = uptr.take_256_ptr();
  vout = uptr.take_32();
  auto s_size = uptr.take_varuint();
  auto s_body = uptr.take_ch_ptr(s_size);
  script = string_view(s_body, s_size);
  seq = uptr.take_32();
}

bool VIN_T::parse(void) {
  // TODO: mt: start resolve
  return true;
}

bool VIN_T::resolve(void) {
  if (vout != COINBASE_vout) {
    auto txno = TxDB->get(u256string_view(*tx_hash));
    if (txno == NOT_FOUND_U32)
      return b_error("txid " + hash2hex(*tx_hash) + " not found.");
  }
  return true;
}
