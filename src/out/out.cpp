#include <time.h>
#include "misc.h"
#include "bk/bk.h"

void out_bk(const BK_T &bk) {
  time_t t = static_cast<time_t>(bk.time);
  char dt[20];
  strftime(dt, 20, "%Y-%m-%d %OH:%OM:%OS", localtime(&t));   ///FUTURE: back into gmtime
  printf("b\t%u\t'%s'\t'%s'\n", bk.height, dt, hash2hex(bk.hash).c_str()); ///FUTURE: s/'hash'/hash/
}

void out_tx(const TX_T &tx) {
  // FIXME: through tx counter
  printf("t\t%u\t%u\t%s\n", tx.no, tx.bk_no, hash2hex(tx.hash).c_str());
}

void out_vin(const VIN_T &vin) {
  if (vin.vout != COINBASE_vout)  // skip coinbase
    printf("i\t%u\t%u\t%u\n", vin.tx_id, vin.vout, vin.tx_no);
}

void out_vout(const VOUT_T &vout) {
  if (vout.addr->is_full())
    printf("o\t%u\t%u\t%" PRIu64 "\t%u\n", vout.tx_no, vout.no, vout.satoshi, vout.addr_id);
  else
    printf("o\t%u\t%u\t%" PRIu64 "\t\\N\n", vout.tx_no, vout.no, vout.satoshi);
}
