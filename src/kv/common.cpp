#include "bce.h"
#include "kv.h"

KV_BASE_T        *TxDB = nullptr, *AddrDB = nullptr;

bool    set_cache(void) {
    if (OPTS.cash) {
        TxDB = new KV_T();
        AddrDB = new KV_T();
        filesystem::path tpath, apath;
        tpath = OPTS.cachedir / TxFileName;
        apath = OPTS.cachedir / AddrFileName;
        if (!TxDB->init(tpath) or !AddrDB->init(apath))
            return false;
        auto tx_full = bool(TxDB->count());
        if (!tx_full and OPTS.from > 0) {
            cerr << "-f > 0 but Tx k-v is empty. Use '-f 0' to clean." << endl;
            return false;
        }
        auto addr_full = bool(AddrDB->count());
        if (!addr_full and OPTS.from > 0) {
            cerr << "-f > 0 but Addr k-v is empty. Use '-f 0' to clean." << endl;
            return false;
        }
        if (tx_full or addr_full) {
            if (OPTS.from == MAX_UINT32) {
                cerr << "Tx (" << TxDB->count() << ") or Addr ("<< AddrDB->count() << ") k-v is not empty. Set -f option." << endl;
                return false;
            } else if (OPTS.from == 0) {
                TxDB->clear();
                AddrDB->clear();
            }
        }
        //
        COUNT.tx = TxDB->count();
        COUNT.addr = AddrDB->count();
    } else {    // not k-v mode
        if (OPTS.from == MAX_UINT32)
            OPTS.from = 0;
    }
    return true;
}

void stop_cache(void)
{
  if (OPTS.cash) {
      TxDB->close();
      AddrDB->close();
  }
}
