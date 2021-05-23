#include "bk/bk.h"
#include "bce.h"      // STAT, OPTS
//#include "uintxxx.h"  // hash

using namespace std;

// == VOUT ==
VOUT_T::VOUT_T(UNIPTR_T &uptr, const uint32_t vout_no) : no(vout_no) {
  satoshi = uptr.take_64();
  auto s_size = uptr.take_varuint();
  auto s_body = uptr.take_ch_ptr(s_size);
  script = string_view(s_body, s_size);
}

bool VOUT_T::parse(void) {
  // TODO: create address
  try {
    addr = addr_decode(script);
  }  catch (AddrException e) {
    cerr << e.what() << endl;
  }
  if (addr)
    STAT.addrs++;
  return true;
}

bool VOUT_T::resolve(void) {
  return true;
}
