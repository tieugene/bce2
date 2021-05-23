#include <iostream>
#include "misc.h"
#include "bk/bk.h"

using namespace std;

void prn_bk(const BK_T &bk) {
  cout
      << "Bk: " << bk.height
      << ", time: " << bk.time
      << ", hash: " << hash2hex(bk.hash)
      << ", txs: " << bk.txs.size()
      << endl;
      // << " (" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")"
  for (auto tx : bk.txs) // TODO: m/t
    prn_tx(tx);
}

void prn_tx(const TX_T &tx) {
  cout
      << "  Tx: " << tx.no
      << " (" << COUNT.tx << ")"
      << ", hash: " << hash2hex(tx.hash)
      << ", ins: "  << tx.vins.size()
      << ", outs: " << tx.vouts.size()
      << endl;
}

void prn_vin(const VIN_T &vint) {
}

void prn_vout(const VOUT_T &vout) {
}
