/*
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
KV_T        *TxDB = nullptr, *AddrDB = nullptr;
long        start_mem;
time_t      start_time;
// locals
KVMEM_T     *TxMEM = nullptr, *AddrMEM = nullptr;
KVKC_T      *TxKC = nullptr, *AddrKC = nullptr;
// consts
const uint32_t  BULK_SIZE = 1000;
// forwards
bool    set_cash(void); ///< setup k-v storages

int     main(int argc, char *argv[])
/* TODO:
 * - local BUFFER
 * [- local FOFF]
 */
{
    // 1. prepare
    // 1.1. handle CLI
    if (!cli(argc, argv))  // no file defined
        return 1;
    auto bk_no_upto = OPTS.from + OPTS.num;
    // 1.2.1. prepare bk info
    auto bk_qty = load_fileoffsets(argv[argc-1]);
    if (!bk_qty)
        return 1;
    if (bk_qty < bk_no_upto) {
        cerr << "Loaded blocks (" << bk_qty << ") < max wanted " << bk_no_upto << endl;
        return 1;
    }
    // 1.2.2. prepare dat
    if (!OPTS.datdir.empty() and OPTS.datdir.back() != '/')
        OPTS.datdir += '/';  // FIXME: native OS path separator
    DATFARM_T datfarm(bk_qty, OPTS.datdir);
    // 1.3. prepare k-v storages
    if (!set_cash())
        return 1;
    // 1.4. last prestart
    BUFFER.beg = new char[MAX_BK_SIZE];
    // 2. main loop
    start_time = time(nullptr);
    start_mem = memused();
    if (OPTS.verbose)
      __prn_head();
    for (COUNT.bk = OPTS.from; COUNT.bk < bk_no_upto; COUNT.bk++)
    {
        // BUSY.tx = BUSY.vin = BUSY.vout = false;
        if (!load_bk(datfarm, FOFF[COUNT.bk].fileno, FOFF[COUNT.bk].offset))
            break;
        if (!parse_bk()) {
            __prn_trace();
            break;
        }
        if ((OPTS.verbose) and (((COUNT.bk+1) % BULK_SIZE) == 0))
            __prn_interim();
        // if ((OPTS.verbose > 3) and ((COUNT.bk % BULK_SIZE) == 0))
        //    cerr << COUNT.bk << endl;
        // CUR_TX.busy = CUR_VIN.busy = CUR_VOUT.busy = false;
    }
    // 3. The end
    if (OPTS.verbose) {
      __prn_tail();
      __prn_interim();
      if (OPTS.verbose > DBG_MIN)
        __prn_summary();
    }
    if (OPTS.inmem and OPTS.cash) { // flush
        TxMEM->cp(TxKC);
        AddrMEM->cp(AddrKC);
    }
    if (BUFFER.beg)
        delete BUFFER.beg;
    return 0;
}

bool    set_cash(void)
{
    if (OPTS.cash or OPTS.inmem) {
        bool tx_full = false, addr_full = false;
        if (OPTS.cash) {
            TxKC = new KVKC_T();
            AddrKC = new KVKC_T();
            if (OPTS.cachedir.back() != '/')
                OPTS.cachedir += '/';  // FIXME: native path separator
            auto s = OPTS.cachedir + "tx.kch";
            if (!TxKC->init(s)) {
                cerr << "Can't open 'tx' cache: " << s << endl;
                return false;
            }
            s = OPTS.cachedir + "addr.kch";
            if (!AddrKC->init(s)) {
                cerr << "Can't open 'addr' cache " << s << endl;
                return false;
            }
            tx_full = bool(TxKC->count());
            addr_full = bool(AddrKC->count());
            if (tx_full != addr_full and OPTS.from > 0) {
                cerr << "-f > 0 but Tx and/or Addr k-v is/are empty. Use '-f 0' to clean." << endl;
                return false;
            }
            if (tx_full or addr_full) {
                if (OPTS.from < 0) {
                    cerr << "Tx or Addr k-v is not empty. Set -f option." << endl;;
                    return false;
                } else if (OPTS.from == 0) {
                    TxKC->clear();
                    tx_full = false;
                    AddrKC->clear();
                    addr_full = false;
                }
            }
            if (!OPTS.inmem) {
                TxDB = TxKC;
                AddrDB = AddrKC;
            }
        }
        if (OPTS.inmem) {
            if (OPTS.cash) {
                if (tx_full) {
                    if (OPTS.verbose)
                        cerr << "Loading txs ...";
                    if (!TxKC->cp(TxMEM)) {
                        cerr << "Loading tx Error." << endl;
                        return false;
                    }
                    if (OPTS.verbose)
                        cerr << "OK." << endl;
                }
                if (addr_full) {
                    if (OPTS.verbose)
                        cerr << "Loading addrs ...";
                    if (!AddrKC->cp(AddrMEM)) {
                        cerr << "Error." << endl;
                        return false;
                    }
                    if (OPTS.verbose)
                        cerr << "OK." << endl;
                }
            } else {    // mem-only
                if (OPTS.from > 0) {
                    cerr << "Can't start from -f > 0 in memory-only mode." << endl;
                    return false;
                } else if (OPTS.from < 0)
                    OPTS.from = 0;
            }
            TxDB = TxMEM;
            AddrDB = AddrMEM;
        }
        COUNT.tx = TxDB->count();
        COUNT.addr = AddrDB->count();
    } else {    // not k-v mode
        if (OPTS.from < 0)
            OPTS.from = 0;
    }
    return true;
}
