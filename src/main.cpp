/*
 * main.cpp
*/

#include "bce.h"
#include "bkidx.h"
#include "misc.h"
#include "handlers.h"
#include "printers.h"

// globals
OPT_T       OPTS;
DBG_LVL_T   DBG_LVL;
COUNT_T     COUNT;
STAT_T      STAT;
LOCAL_T     LOCAL;
BUSY_T      BUSY;
BK_T        CUR_BK;
TX_T        CUR_TX;
VIN_T       CUR_VIN;
VOUT_T      CUR_VOUT;
UNIPTR_T    CUR_PTR;
BUFFER_T    BUFFER;
long        start_mem;
time_t      start_time;
// locals
static time_t T0;
// consts
const uint32_t  BULK_SIZE = 1000;

int     main(int argc, char *argv[])
{
  bool (*bkloader)(char *, const uint32_t) = &stdin_bk;

  // TODO: local BUFFER
    T0 = time(nullptr);
    // 1. prepare
    // 1.1. handle CLI
    if (!cli(argc, argv))
        return 1;
    // 1.2. prepare bk info
    if (OPTS.datdir != FROM_STDIN) {
      auto bk_qty = init_bkloader(OPTS.datdir, OPTS.locsfile);
      if (!bk_qty)
        return 2;
      if (bk_qty <= OPTS.from) {
          cerr << "Loaded blocks (" << bk_qty << ") <= 'from' " << OPTS.from << endl;
          return 3;
      }
      bkloader = load_bk;
    }
    // 1.3. prepare bk buffer
    BUFFER.beg = new char[MAX_BK_SIZE];
    // 1.4. prepare k-v storages (and normalize OPTS.from)
    start_mem = memused();
    if (!set_cache())
        return 1;
    // 1.5. last prestart
    if (OPTS.verbose)
      __prn_head();
    start_time = time(nullptr);
    // 2. main loop
    for (COUNT.bk = OPTS.from; bkloader(BUFFER.beg, COUNT.bk); COUNT.bk++) {
      CUR_PTR.v_ptr = BUFFER.beg;
      if (!parse_bk()) {
          __prn_trace();
          break;
      }
      if ((OPTS.verbose) and (((COUNT.bk+1) % BULK_SIZE) == 0))
          __prn_interim();
      if (OPTS.num)   // not 'untill the end'
        if (--OPTS.num == 0)
          break;
    }
    // 3. The end
    if (OPTS.verbose) {
      __prn_tail();
      __prn_interim();
      if (OPTS.verbose > DBG_MIN)
        __prn_summary();
    }
    stop_cache();
    if (BUFFER.beg)
        delete BUFFER.beg;
    return 0;
}
