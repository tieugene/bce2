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
static KV_T *TxMEM = nullptr, *AddrMEM = nullptr;
static KV_T *TxKC = nullptr, *AddrKC = nullptr;
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
    // 1.2.1. prepare bk info
    auto bk_qty = load_fileoffsets(argv[argc-1]);
    if (!bk_qty)
        return 1;
    auto bk_no_upto = (OPTS.from < 0 ? 0 : OPTS.from) + OPTS.num;
    if (bk_qty < bk_no_upto) {
        cerr << "Loaded blocks (" << bk_qty << ") < max wanted " << bk_no_upto << endl;
        return 1;
    }
    // 1.2.2. prepare dat
    if (!OPTS.datdir.empty() and OPTS.datdir.back() != '/')
        OPTS.datdir += '/';  // FIXME: native OS path separator
    DATFARM_T datfarm(bk_qty, OPTS.datdir);
    // 1.2.3. prepare bk buffer
    BUFFER.beg = new char[MAX_BK_SIZE];
    // 1.3. prepare k-v storages (and normalize OPTS.from)
    start_mem = memused();
    if (!set_cash())
        return 1;
    // 1.4. last prestart
    if (OPTS.verbose)
      __prn_head();
    start_time = time(nullptr);
    // 2. main loop
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
        if (OPTS.verbose) {
            // tx
            cerr << "Flush tx   (" << TxKC->count() << " => ";
            auto t = time(nullptr);
            TxMEM->cpto(TxKC);
            cerr << TxKC->count() << " @ " << time(nullptr)-t << "s OK." << endl;
            // addr
            cerr << "Flush addr (" << AddrKC->count() << " => ";
            t = time(nullptr);
            AddrMEM->cpto(AddrKC);
            cerr << AddrKC->count() << " @ " << time(nullptr)-t << "s OK." << endl;
        } else {
            TxMEM->cpto(TxKC);
            AddrMEM->cpto(AddrKC);
        }
    }
    if (BUFFER.beg)
        delete BUFFER.beg;
    return 0;
}

bool    set_cash(void)
{
    if (kv_mode()) {
        bool tx_full = false, addr_full = false;
        if (OPTS.cash) {
            TxKC = new KV_T();
            AddrKC = new KV_T();
            string tpath, apath;
            if (OPTS.cachedir.size() == 1)  {   // on-memory
                tpath = apath = OPTS.cachedir;
            } else {
                if (OPTS.cachedir.back() != '/')
                    OPTS.cachedir += '/';  // FIXME: native path separator
                tpath = OPTS.cachedir + "tx.kch";
                apath = OPTS.cachedir + "addr.kch";
            }
            if (!TxKC->init(tpath) or !AddrKC->init(apath))
                return false;
            tx_full = bool(TxKC->count());
            addr_full = bool(AddrKC->count());
            if (tx_full != addr_full and OPTS.from > 0) {
                cerr << "-f > 0 but Tx and/or Addr k-v is/are empty. Use '-f 0' to clean." << endl;
                return false;
            }
            if (tx_full or addr_full) {
                if (OPTS.from < 0) {
                    cerr << "Tx (" << TxKC->count() << ") or Addr ("<< AddrKC->count() << ") k-v is not empty. Set -f option." << endl;
                    return false;
                } else if (OPTS.from == 0) {
                    TxKC->clear();
                    tx_full = false;
                    AddrKC->clear();
                    addr_full = false;
                }
            }
            if (!OPTS.inmem) {
                if (OPTS.verbose) {
                    TxDB = TxKC;
                    AddrDB = AddrKC;
                }
            }
        }
        if (OPTS.inmem) {
            TxMEM = new KV_T();
            TxMEM->init(":");   // StashDB
            AddrMEM = new KV_T();
            AddrMEM->init(":");
            if (OPTS.cash) {
                if (tx_full) {
                    if (OPTS.verbose)
                        cerr << "Loading txs ...";
                    auto t = time(nullptr);
                    if (!TxKC->cpto(TxMEM)) {
                        cerr << "Loading tx Error." << endl;
                        return false;
                    }
                    if (OPTS.verbose)
                        cerr << time(nullptr)-t << "s OK." << endl;
                }
                if (addr_full) {
                    if (OPTS.verbose)
                        cerr << "Loading addrs ...";
                    auto t = time(nullptr);
                    if (!AddrKC->cpto(AddrMEM)) {
                        cerr << "Error." << endl;
                        return false;
                    }
                    if (OPTS.verbose)
                        cerr << time(nullptr)-t << "s OK." << endl;
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
