#include "bk/bk.h"
#include "bce.h"      // STAT, OPTS
#include "uintxxx.h"  // hash

using namespace std;

TX_T::TX_T(UNIPTR_T &uptr) {
  auto tx_beg = uptr.ch_ptr;
  ver = uptr.take_32();
  bool segwit = (*uptr.u16_ptr == 0x0100);
  if (segwit)
    uptr.u16_ptr++;  // skip witness signature
  // vins
  auto vin_count = uptr.take_varuint();    // vins
  if (vin_count == 0)
    throw BCException("Vins == 0");
  for (uint32_t i = 0; i < vin_count; i++)
    vins.push_back(VIN_T(uptr));
  auto vout_count = uptr.take_varuint();   // vouts
  for (uint32_t i = 0; i < vout_count; i++)
    vouts.push_back(VOUT_T(uptr));
  if (segwit)                         // wits
      for (uint32_t i = 0; i < vin_count; i++)
          wits.push_back(WIT_T(uptr));
  uptr.take_32();                     // skip locktime
  data = string_view(tx_beg, uptr.ch_ptr - tx_beg);
  // Counters
  STAT.vins += vin_count;
  STAT.vouts += vout_count;
  STAT.max_vins = max(STAT.max_vins, vin_count);
  STAT.max_vouts = max(STAT.max_vouts, vout_count);
}

bool TX_T::parse(void) {
  // [start] calc hash (on demand)
  bool retvalue(true);
  // for (auto it_vin = vins.begin(); it_vin != vins.end(); it_vin++)
  //  revalue &= it_vin->parse();
  for (auto it_vout = vouts.begin(); it_vout != vouts.end(); it_vout++)
    retvalue &= it_vout->parse();
  return retvalue;
}

bool TX_T::resolve(void) {
  bool retvalue(true);
  for (auto it_vin = vins.begin(); it_vin != vins.end(); it_vin++)
    retvalue &= it_vin->parse();
  for (auto it_vout = vouts.begin(); it_vout != vouts.end(); it_vout++)
    retvalue &= it_vout->parse();
  return retvalue;
}
