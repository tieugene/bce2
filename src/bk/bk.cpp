#include "bk/bk.h"
#include "bce.h"  // STAT, OPTS

using namespace std;

static uint32_t BK_GLITCH[] = {91722, 91842};    // dup of 91880, 91812
/// Bk header (80 bytes)
struct  BK_HEAD_T {
  uint32_t    ver;
  uint256_t   p_hash;
  uint256_t   mroot;
  uint32_t    time;
  uint32_t    bits;
  uint32_t    nonce;
};

BK_T::BK_T(string_view src, uint32_t bk_no) : height(bk_no), data(src) {
  UNIPTR_T uptr(data.cbegin());
  uptr.take_u8_ptr(sizeof(BK_HEAD_T)); // skip header
  auto tx_count = uptr.take_varuint();
  if (!(bk_no == BK_GLITCH[0] or bk_no == BK_GLITCH[1]))  // skip glitch blocks
    for (uint32_t i = 0; i < tx_count; i++, COUNT.tx++)
      tx.push_back(TX_T(uptr));
  // Counters
  STAT.max_txs = max(STAT.max_txs, tx_count);
}

bool BK_T::parse(void) {
  // apply header
  // calc hash (on demand)
  // parse txs (m/t)
  return false;
}

TX_T::TX_T(UNIPTR_T &uptr) {
  auto tx_beg = uptr.ch_ptr;
  ver = uptr.take_32();
  bool segwit = (*uptr.u16_ptr == 0x0100);
  if (segwit)
    uptr.u16_ptr++;  // skip witness signature
  // vins
  auto vins = uptr.take_varuint();    // vins
  if (vins == 0)
    throw BCException("Vins == 0");
  for (uint32_t i = 0; i < vins; i++)
    vin.push_back(VIN_T(uptr));
  auto vouts = uptr.take_varuint();   // vouts
  for (uint32_t i = 0; i < vouts; i++)
    vout.push_back(VOUT_T(uptr));
  if (segwit)                         // wits
      for (uint32_t i = 0; i < vins; i++)
          wit.push_back(WIT_T(uptr));
  uptr.take_32();                     // skip locktime
  data = string_view(tx_beg, uptr.ch_ptr - tx_beg);
  // Counters
  STAT.vins += vins;
  STAT.vouts += vouts;
  STAT.max_vins = max(STAT.max_vins, vins);
  STAT.max_vouts = max(STAT.max_vouts, vouts);
}

VIN_T::VIN_T(UNIPTR_T &uptr) {
  tx_hash = uptr.take_256_ptr();
  vout = uptr.take_32();
  auto s_size = uptr.take_varuint();
  auto s_body = uptr.take_ch_ptr(s_size);
  script = string_view(s_body, s_size);
  seq = uptr.take_32();
}

VOUT_T::VOUT_T(UNIPTR_T &uptr) {
  satoshi = uptr.take_64();
  auto s_size = uptr.take_varuint();
  auto s_body = uptr.take_ch_ptr(s_size);
  script = string_view(s_body, s_size);
}

WIT_T::WIT_T(UNIPTR_T &uptr) {
  auto count = uptr.take_varuint();
  for (uint32_t i = 0; i < count; i++)
    uptr.u8_ptr += uptr.take_varuint();
}
