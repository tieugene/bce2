#include "bk/bk.h"
//#include "bce.h"      // STAT, OPTS
//#include "uintxxx.h"  // hash

using namespace std;

// == WIT ==
WIT_T::WIT_T(UNIPTR_T &uptr, const uint32_t no, const uint32_t tx_no, const uint32_t bk_no)
  : no(no), tx_no(tx_no), bk_no(bk_no) {
  auto count = uptr.take_varuint();
  for (uint32_t i = 0; i < count; i++)
    uptr.u8_ptr += uptr.take_varuint();
}
