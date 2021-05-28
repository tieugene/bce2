/*
 * main.cpp
*/

#include "bce.h"
#include "misc.h"
#include "load/bkidx.h"
#include "bk/bk.h"
#include "out/log.h"

// globals
OPT_T       OPTS;
STAT_T      STAT;
COUNT_T     COUNT;
extern long        start_mem;
extern time_t      start_time;

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
    if (OPTS.verbose > DBG_MIN) {
      log_opts();
      log_head();
    }
    start_time = time(nullptr);
    // 2. main loop
    for (COUNT.bk = OPTS.from; OPTS.num; COUNT.bk++) { // FIXME: cond
      auto buffer = bkloader(COUNT.bk); // 1. load
      if (buffer.empty())
        break;
      auto bk = BK_T(buffer, COUNT.bk); // 2. create objects
      if (!bk.parse())                  // 3. parse
        break;
      if (kv_mode()) {
        if (!bk.resolve())              // 4. resolve
          break;
        if (OPTS.out)                   // 5. out
          out_bk(bk);
      } else {
        if (OPTS.out)
          prn_bk(bk);
      }
      if ((OPTS.verbose) and (((COUNT.bk+1) % OPTS.logstep) == 0))  // 7. log
          log_interim();
      if (OPTS.num)   // not 'untill the end'
        if (--OPTS.num == 0)
          break;
    }
    // 3. The end
    if (OPTS.verbose > DBG_MIN) {
      log_tail();
      log_interim();
      log_summary();
    }
    stop_cache();
    return 0;
}
