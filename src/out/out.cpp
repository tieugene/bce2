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
}

void out_vin(const VIN_T &vin) {
}

void out_vout(const VOUT_T &vout) {
}
