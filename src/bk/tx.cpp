#include <vector>
#include "bce.h"      // STAT, OPTS
#include "crypt/uintxxx.h"  // hash
#include "bk/bk.h"

using namespace std;

TX_T::TX_T(UNIPTR_T &uptr, const uint32_t no, BK_T * const bk)
  : bk(bk), no(no) {
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
    vins.push_back(make_unique<VIN_T>(uptr, i, this));
  auto vout_count = uptr.take_varuint();  // vouts
  for (uint32_t i = 0; i < vout_count; i++)
    vouts.push_back(make_unique<VOUT_T>(uptr, i, this));
  wit_offset = uptr.ch_ptr - tx_beg;
  if (segwit)                             // wits
    for (uint32_t i = 0; i < vin_count; i++)
      wits.push_back(make_unique<WIT_T>(uptr, i, this));
  uptr.take_32();                         // skip locktime
  data = string_view(tx_beg, uptr.ch_ptr - tx_beg);
  // Counters
  STAT.vins += vin_count;
  STAT.vouts += vout_count;
  STAT.max_vins = max(STAT.max_vins, vin_count);
  STAT.max_vouts = max(STAT.max_vouts, vout_count);
  // cerr << "+TX " << to_string(no) << endl;
}

const string TX_T::err_prefix(void) {
  return "Tx # " + std::to_string(no) + ": ";
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
    // hash256(hash_src, hash);
  }
}

bool TX_T::parse(void) {
  if (OPTS.out or kv_mode())
    mk_hash();  // TODO: on demand/mt
  bool retvalue(true);
  // for (auto &vin ; vins)
  //  revalue &= vin.parse();
  for (auto &vout : vouts)
    retvalue &= vout->parse();
  if (retvalue and !kv_mode())
    COUNT.tx++;   // session counter
  return retvalue;
}

bool TX_T::resolve(void) {
  bool retvalue = ((id = TxDB->add(u256string_view(hash))) != MAX_UINT32);
  if (retvalue and (id != COUNT.tx)) {
    retvalue = b_error(err_prefix() + "new tx is # " + to_string(id) + " instead of expecting " + to_string(COUNT.tx));
    rollback(false);
  }
  if (retvalue)
    for (auto &vin : vins)
      if (!(retvalue &= vin->resolve()))
        break;
  if (retvalue)
    for (auto &vout : vouts)
      if (!(retvalue &= vout->resolve()))
        break;
  if (retvalue)
    COUNT.tx++;
  else {
    v_error(err_prefix() + "Resolve oops");
    rollback();
  }
  return retvalue;
}

bool TX_T::rollback(bool recur) {
  bool retvalue(true);
  if (id != MAX_UINT32) {
    if (OPTS.verbose == DBG_MAX)
      v_error(err_prefix() + "Rolling back...");
    retvalue = TxDB->del(u256string_view(hash));
    if (retvalue)
      id = MAX_UINT32;
    else
      v_error(err_prefix() + "Cannot rollback itself.");
    if (recur)
      for (auto &vout : vouts)
        retvalue &= vout->rollback();
  }
  if (!retvalue)
    v_error(err_prefix() + "Rolling back oops.");
  return retvalue;
}
