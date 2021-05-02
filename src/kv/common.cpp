#include "bce.h"
#include "kv.h"

KV_T        *TxDB = nullptr, *AddrDB = nullptr;
static KV_T *TxMEM = nullptr, *AddrMEM = nullptr;
static KV_T *TxKC = nullptr, *AddrKC = nullptr;

bool    set_cache(void)
{
    if (kv_mode()) {
        bool tx_full = false, addr_full = false;
        if (OPTS.cash) {    // any cache
            TxKC = new KV_T();
            AddrKC = new KV_T();
            string tpath, apath;
            if (OPTS.cachedir.size() == 1)  {   // FIXME: 'on-memory; for tests'
                tpath = apath = OPTS.cachedir;
            } else {
                if (OPTS.cachedir.back() != '/')
                    OPTS.cachedir += '/';  // FIXME: native path separator
                tpath = OPTS.cachedir + TxFileName;
                apath = OPTS.cachedir + AddrFileName;
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
                TxDB = TxKC;
                AddrDB = AddrKC;
            }
        }
        if (OPTS.inmem) {
            TxMEM = new KV_T();
            TxMEM->init(TxMemName);   // StashDB
            AddrMEM = new KV_T();
            AddrMEM->init(AddrMemName);
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

void stop_cache(void)
{
  if (OPTS.cash) {
      if (OPTS.inmem) { // flush
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
      TxKC->close();
      AddrKC->close();
  }
}
