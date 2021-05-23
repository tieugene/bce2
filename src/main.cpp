/*
 * main.cpp
*/

#include "bce.h"
#include "load/bkidx.h"
#include "misc.h"
#include "bk/bk.h"
#include "bk/handlers.h"
#include "out/printers.h"

// globals
OPT_T       OPTS;
COUNT_T     COUNT;
STAT_T      STAT;
LOCAL_T     LOCAL;
BUSY_T      BUSY;
BK_OLD_T        CUR_BK;
TX_OLD_T        CUR_TX;
VIN_OLD_T       CUR_VIN;
VOUT_OLD_T      CUR_VOUT;
UNIPTR_T    CUR_PTR;
long        start_mem;
time_t      start_time;

using namespace std;

int     main(int argc, char *argv[]) {
  string_view (*bkloader)(const uint32_t) = &stdin_bk;

    // 1. prepare
    // 1.1. handle options
    if (!load_opts(argc, argv))
      return u32_error("Load_opts oops.", 1);
    // 1.2. prepare bk info
    if (!OPTS.fromcin) {
      auto bk_qty = init_bkloader(OPTS.datdir, OPTS.locsfile);
      if (!bk_qty)
        return u32_error("Init_bkloader oops.", 2);
      if ((OPTS.from != MAX_UINT32) and (bk_qty <= OPTS.from))
        return u32_error("Loaded blocks (" + to_string(bk_qty) + ") <= 'from' " + to_string(OPTS.from), 3);
      bkloader = load_bk;
    }
    // 1.3. prepare k-v storages (and normalize OPTS.from)
    start_mem = memused();
    if (!set_cache())
        return u32_error("Set_cache oops.", 3);
    // 1.4. last prestart
    if (OPTS.verbose)
      __prn_head();
    start_time = time(nullptr);
    // 2. main loop
    for (COUNT.bk = OPTS.from; OPTS.num; COUNT.bk++) { // FIXME: cond
      auto buffer = bkloader(COUNT.bk);
      if (buffer.empty())
        break;
      auto bk = BK_T(buffer, COUNT.bk);
      bk.parse();
      //CUR_PTR.v_ptr = buffer.begin();
      //auto parsed_ok = parse_bk();
      delete buffer.begin();
      /*
      if (!parsed_ok) {
          v_error("Bk # " + to_string(COUNT.bk));
          break;
      }*/
      if ((OPTS.verbose) and (((COUNT.bk+1) % OPTS.logstep) == 0))
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
    return 0;
}
