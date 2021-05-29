#include <iostream>
#include "misc.h"
#include "bk/bk.h"

using namespace std;

void prn_bk(const BK_T &bk) {
  // TODO: ISO datime put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS")
#ifndef ADDR_ONLY
  printf("B: # %u, Time: %u, Txs: %lu, Hash: %s\n",
    bk.height, bk.time, bk.txs.size(), hash2hex(bk.hash).c_str());
#endif
  for (auto &tx : bk.txs)
    prn_tx(*tx);
}

void prn_tx(const TX_T &tx) {
  // TODO: through no (COUNT.tx/tx.id)
#ifndef ADDR_ONLY
  printf(" T: # %u, Ins: %lu, Outs: %lu, SegWit: %d, Hash: %s\n",
    tx.no, tx.vins.size(), tx.vouts.size(), tx.segwit, hash2hex(tx.hash).c_str());
  for (auto &vin : tx.vins)
    prn_vin(*vin);
#endif
  for (auto &vout : tx.vouts)
    prn_vout(*vout);
}

void prn_vin(const VIN_T &vin) {
  // TODO: if vin.vout == 0xFFFFFFFF ? <coinbase> : tx+no.vout
  if (vin.vout == COINBASE_vout)
    printf("  <: # %u, Src: <coinbase>\n", vin.no);
  else {
    if (vin.tx_id == MAX_UINT32)
      printf("  <: # %u, Src: tx %s, vout %u\n", vin.no, hash2hex(*vin.tx_hash).c_str(), vin.vout);
    else
      printf("  <: # %u, Src: tx %u, vout %u\n", vin.no, vin.tx_id, vin.vout);
  }
}

void prn_vout(VOUT_T &vout) {
#ifndef ADDR_ONLY
  if (vout.addr and vout.addr->is_full())
    printf("  >: # %u, $: %llu, Addr: %s %s\n", vout.no, vout.satoshi, vout.addr_type().c_str(), vout.addr->repr().c_str());
  else
    printf("  >: # %u, $: %llu, Addr: %s %lu bytes\n", vout.no, vout.satoshi, vout.addr_type().c_str(), vout.script.length());
#else
  auto a = vout.addr_repr();
  if (!a.empty())
    printf("%d\t%d\t%d\t%s\t%s\n", vout.bk_no, vout.tx_no, vout.no, vout.addr_type().c_str(), a.c_str());
  else
    printf("%d\t%d\t%d\t%s\n", vout.bk_no, vout.tx_no, vout.no, vout.addr_type().c_str());
#endif
}
