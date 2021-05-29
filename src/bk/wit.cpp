#include "bk/bk.h"

using namespace std;

// == WIT ==
WIT_T::WIT_T(UNIPTR_T &uptr, const uint32_t no, TX_T * const tx)
    : tx(tx), no(no) {
  auto count = uptr.take_varuint();
  for (uint32_t i = 0; i < count; i++)
    uptr.u8_ptr += uptr.take_varuint();
}
