/*
Usage: [options] file
Options:
-f[rom]     - start block
-n[um]      - blocks to process
-d[atdir]   - *.dat-files folder
-c[ache]    - helping data folder
-q[uiet]    - no output result (stdout)
-v[erbose]  - debug info (stderr)
-k[eep]     - keep existing data
-h[elp]     - subj
*/

#include "bce.h"
#include "bkidx.h"
#include "misc.h"
#include "handlers.h"
#include "printers.h"

// globals
UNIPTR_T    CUR_PTR;
OPT_T       OPTS;
STAT_T      STAT;
BK_T        CUR_BK;
TX_T        CUR_TX;
VIN_T       CUR_VIN;
VOUT_T      CUR_VOUT;
BUFFER_T    BUFFER;
#ifdef MEM
TxMAP_T    TxDB;
AddrMAP_T  AddrDB;
#else
TxDB_T    TxDB;
AddrDB_T  AddrDB;
#endif
time_t    start_time;
// locals
// consts
const uint32_t  BULK_SIZE = 10000;
// forwards

int     main(int argc, char *argv[])
/* TODO:
 * - local BUFFER
 * [- local FOFF]
 */
{
    // 1. handle CLI
    if (!cli(argc, argv))  // no file defined
        return 1;
    auto bk_no_upto = OPTS.from + OPTS.num;
    // 1.1. prepare bk info
    auto bk_qty = load_fileoffsets(argv[argc-1]);
    if (!bk_qty)
        return 1;
    if (bk_qty < bk_no_upto) {
        cerr << "Loaded blocks (" << bk_qty << ") < max wanted " << bk_no_upto << endl;
        return 1;
    }
    // 1.2. prepare dat
    if (!OPTS.datdir.empty() and OPTS.datdir.back() != '/')
        OPTS.datdir += '/';  // FIXME: native OS path separator
    DATFARM_T datfarm(bk_qty, OPTS.datdir);
    // 1.3. prepare k-v storages
    if (job_mode()) {
        if (OPTS.cachedir.back() != '/')
            OPTS.cachedir += '/';  // FIXME: native path separator
        auto s = OPTS.cachedir + "tx.kch";
        if (!TxDB.init(s)) {
            cerr << "Can't open 'tx' cache: " << s << endl;
            return 1;
        }
        s = OPTS.cachedir + "addr.kch";
        if (!AddrDB.init(s)) {
            cerr << "Can't open 'addr' cache " << s << endl;
            return 1;
        }
        if (OPTS.from == 0) {
          TxDB.clear();
          AddrDB.clear();
        }
    }
    // 1.4. last prestart
    BUFFER.beg = new char[MAX_BK_SIZE];
    // 2. main loop
    start_time = time(nullptr);
    if (OPTS.verbose)
      __prn_head();
    for (auto i = OPTS.from; i < bk_no_upto; i++)
    {
        CUR_TX.busy = CUR_VIN.busy = CUR_VOUT.busy = false;
        CUR_BK.busy = true;
        CUR_BK.no = i;
        if (!load_bk(datfarm, FOFF[i].fileno, FOFF[i].offset))
            break;
        if (!parse_bk()) {
            __prn_trace();
            break;
        }
        if ((OPTS.verbose) and (((CUR_BK.no+1) % BULK_SIZE) == 0))
            __prn_interim();
        if ((OPTS.verbose > 3) and ((i % BULK_SIZE) == 0))
            cerr << i << endl;
        CUR_TX.busy = CUR_VIN.busy = CUR_VOUT.busy = false;
        CUR_BK.busy = false;
    }
    // x. The end
    if (OPTS.verbose) {
      __prn_tail();
      __prn_interim();
      if (OPTS.verbose >= 2)
        __prn_summary();
    }
    if (BUFFER.beg)
        delete BUFFER.beg;
    return 0;
}
