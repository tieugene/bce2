#include <time.h>
#include "misc.h"
#include "bk/bk.h"

void out_bk(const BK_T &bk) {
  time_t t = static_cast<time_t>(bk.time);
  char dt[20];
  strftime(dt, 20, "%Y-%m-%d %OH:%OM:%OS", localtime(&t));   ///FUTURE: back into gmtime
  printf("b\t%u\t'%s'\t'%s'\n", bk.height, dt, hash2hex(bk.hash).c_str()); ///FUTURE: s/'hash'/hash/
  for (auto tx : bk.txs)
    out_tx(*tx);
}

void out_tx(const TX_T &tx) {
  // FIXME: through tx counter
  printf("t\t%u\t%u\t%s\n", tx.id, tx.bk->get_id(), hash2hex(tx.hash).c_str());
  for (auto vin : tx.vins)
    out_vin(*vin);
  for (auto vout : tx.vouts)
    out_vout(*vout);
}

void out_vin(const VIN_T &vin) {
  if (vin.vout != COINBASE_vout)  // skip coinbase
    printf("i\t%u\t%u\t%u\n", vin.tx_id, vin.vout, vin.tx->get_id());
}

void out_vout(const VOUT_T &vout) {
  if (vout.addr and vout.addr->is_full()) {
    out_addr(vout);
    printf("o\t%u\t%u\t%" PRIu64 "\t%u\n", vout.tx->get_id(), vout.no, vout.satoshi, vout.addr_id);
  } else
    printf("o\t%u\t%u\t%" PRIu64 "\t\\N\n", vout.tx->get_id(), vout.no, vout.satoshi);
}

void out_addr(const VOUT_T &vout) {
  static uint32_t printed_addrs = 0;  ///< sequential counter (FIXME: фуфель, работает только с 0)
  if ((vout.addr_id+1) >= printed_addrs) {
    printf("a\t%u\t%s\t%u\n", vout.addr_id, vout.addr->as_json().c_str(), vout.addr->qty());
    printed_addrs++;
  }
}
