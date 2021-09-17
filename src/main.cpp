/*
 * main.cpp
*/

#include "misc.h"
#include "cfg/opts.h"
#include "load/bkidx.h"
#include "bk/bk.h"
#include "out/log.h"

// globals
OPT_T       OPTS;
STAT_T      STAT;
COUNT_T     COUNT;
extern long        start_mem;   // out/log.cpp
extern time_t      start_time;  // out/log.cpp

using namespace std;

int     main(int argc, char *argv[]) {
  uniq_view (*bkloader)(const uint32_t) = &stdin_bk;

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
    if (!set_cache()) {
      stop_cache();
      return u32_error("Set_cache oops.", 3);
    }
    if (OPTS.verbose > DBG_MIN)
      log_opts();
    if (OPTS.info) {
      stop_cache();
      return 0;
    }
    // 1.4. last prestart
    if (OPTS.verbose)
      log_head();
    start_time = time(nullptr);
    // 2. main loop
    for (COUNT.bk = OPTS.from; OPTS.num; COUNT.bk++, OPTS.num--) {
      auto buffer = bkloader(COUNT.bk); // 1. load
      if (!buffer.first or (buffer.second == 0))
        break;
      auto bk = BK_T(move(buffer.first), COUNT.bk); // 2. create objects
      if (!bk.parse())                  // 3. parse
        break;
      if (kv_mode()) {
        if (!bk.resolve())              // 4. resolve
          break;
        if (!StampDB->update())
          break;
        if (OPTS.out)                   // 5. out
          out_bk(bk);
      } else {
        if (OPTS.out)
          prn_bk(bk);
      }
      if ((OPTS.verbose) and (((COUNT.bk+1) % OPTS.logstep) == 0))  // 7. log
        log_interim();
    }
    stop_cache();
    COUNT.bk--;
    // 3. The end
    if (OPTS.verbose) {
      // log_tail();
      log_interim();
      if (OPTS.verbose > DBG_MIN)
        log_summary();
    }
    return 0;
}
