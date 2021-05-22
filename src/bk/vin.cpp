#include "bk/bk.h"
//#include "bce.h"      // STAT, OPTS
//#include "uintxxx.h"  // hash

using namespace std;

// == VIN ==
VIN_T::VIN_T(UNIPTR_T &uptr) {
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
  return true;
}
