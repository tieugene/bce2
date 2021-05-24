#include <iostream>
#include "misc.h"
#include "bk/bk.h"

using namespace std;

void prn_bk(const BK_T &bk) {
#ifndef ADDR_ONLY
  cout
      << "Bk: " << bk.height
      << ", time: " << bk.time
      << ", hash: " << hash2hex(bk.hash)
      << ", txs: " << bk.txs.size()
      << endl;
      // << " (" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")"
#endif
  for (auto tx : bk.txs) // TODO: m/t
    prn_tx(*tx);
}

void prn_tx(const TX_T &tx) {
#ifndef ADDR_ONLY
  cout
      << "  Tx: " << tx.no
      << " (" << COUNT.tx << ")"
      << ", hash: " << hash2hex(tx.hash)
      << ", ins: "  << tx.vins.size()
      << ", outs: " << tx.vouts.size()
      << endl;
  for (auto vin : tx.vins)
    prn_vin(vin);
#endif
  for (auto vout : tx.vouts)
    prn_vout(*vout);
}

void prn_vin(const VIN_T &vin) {
#ifndef ADDR_ONLY
  cout << "    Vin: " << vin.no
      << ", src: ";
  if (vout == 0xFFFFFFFF)
      cout << "<coinbase>";
  else
      cout << "(Tx: " << tx_no << ", vout: " << vout << ")";
  cout
      << ", ssize: " << script.length()
      << endl;
#endif
}

void prn_vout(VOUT_T &vout) {
#ifndef ADDR_ONLY
  cout
      << "    Vout: " << vout
      << ", $: " << satoshi
      << ", ssize: " << scipt.lengths()
      << endl;
#else
  auto a = vout.addr_repr();
  if (!a.empty())
    printf("%d\t%d\t%d\t%s\t%s\n", vout.bk_no, vout.tx_no, vout.no, vout.addr_type().c_str(), a.c_str());
  else
    printf("%d\t%d\t%d\t%s\n", vout.bk_no, vout.tx_no, vout.no, vout.addr_type().c_str());
#endif
}
