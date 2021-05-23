#include <vector>
#include "bk/bk.h"
#include "bce.h"      // STAT, OPTS
#include "uintxxx.h"  // hash

using namespace std;

TX_T::TX_T(UNIPTR_T &uptr) {
  auto tx_beg = uptr.ch_ptr;
  ver = uptr.take_32();
  segwit = (*uptr.u16_ptr == 0x0100);
  if (segwit)
    uptr.u16_ptr++;  // skip witness signature
  // vins
  auto vin_count = uptr.take_varuint();   // vins
  if (vin_count == 0)
    throw BCException("Vins == 0");
  for (uint32_t i = 0; i < vin_count; i++)
    vins.push_back(VIN_T(uptr));
  auto vout_count = uptr.take_varuint();  // vouts
  for (uint32_t i = 0; i < vout_count; i++)
    vouts.push_back(VOUT_T(uptr));
  wit_offset = uptr.ch_ptr - tx_beg;
  if (segwit)                             // wits
      for (uint32_t i = 0; i < vin_count; i++)
          wits.push_back(WIT_T(uptr));
  uptr.take_32();                         // skip locktime
  data = string_view(tx_beg, uptr.ch_ptr - tx_beg);
  // Counters
  STAT.vins += vin_count;
  STAT.vouts += vout_count;
  STAT.max_vins = max(STAT.max_vins, vin_count);
  STAT.max_vouts = max(STAT.max_vouts, vout_count);
}

void TX_T::mk_hash(void) {  // const u8_t *tx_beg
  if (!segwit)
    hash256(data, hash);
  else {
    vector<char> hash_src(data.begin(), data.end());
    hash_src.erase(hash_src.begin() + wit_offset, hash_src.end() - 4);  // cut off wits
    auto cut_off = hash_src.begin() + sizeof(uint32_t);
    hash_src.erase(cut_off, cut_off + sizeof (uint16_t));               // ...and wit signature
    hash_src.shrink_to_fit();
    hash256(string_view(hash_src.data(), hash_src.size()), hash);
  }
}

bool TX_T::parse(void) {
  mk_hash();  // TODO: on demand/mt
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
    retvalue &= it_vin->resolve();
  for (auto it_vout = vouts.begin(); it_vout != vouts.end(); it_vout++)
    retvalue &= it_vout->resolve();
  return retvalue;
}
