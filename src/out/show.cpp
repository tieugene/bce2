#include <iostream>
#include "misc.h"
#include "bk/bk.h"

using namespace std;

void prn_bk(const BK_T &bk) {
  // TODO: ISO datime put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS")
#ifndef ADDR_ONLY
  printf("B: %u, Time: %u, Txs: %lu, Hash: %s\n",
    bk.height, bk.time, bk.txs.size(), hash2hex(bk.hash).c_str());
#endif
  for (auto tx : bk.txs)
    prn_tx(*tx);
}

void prn_tx(const TX_T &tx) {
  // TODO: through no (COUNT.tx/tx.id)
#ifndef ADDR_ONLY
  printf(" T: %u (%u), Ins: %lu, Outs: %lu, Hash: %s\n",
    tx.no, COUNT.tx, tx.vins.size(), tx.vouts.size(), hash2hex(tx.hash).c_str());
  for (auto vin : tx.vins)
    prn_vin(*vin);
#endif
  for (auto vout : tx.vouts)
    prn_vout(*vout);
}

void prn_vin(const VIN_T &vin) {
  // TODO: if vin.vout == 0xFFFFFFFF ? <coinbase> : tx+no.vout
  printf("  <: %u, Src: %s, s_size: %lu\n", vin.no, "TODO", vin.script.length());
}

void prn_vout(const VOUT_T &vout) {
#ifndef ADDR_ONLY
  // TODO: address
  printf("  >: %u, $: %llu, s_size: %lu\n", vout.no, vout.satoshi, vout.script.length());
#else
  auto a = vout.addr_repr();
  if (!a.empty())
    printf("%d\t%d\t%d\t%s\t%s\n", vout.bk_no, vout.tx_no, vout.no, vout.addr_type().c_str(), a.c_str());
  else
    printf("%d\t%d\t%d\t%s\n", vout.bk_no, vout.tx_no, vout.no, vout.addr_type().c_str());
#endif
}
