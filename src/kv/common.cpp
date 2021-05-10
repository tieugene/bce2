#include <filesystem>

#include "bce.h"
#include "kv.h"

KV_BASE_T  *TxDB = nullptr, *AddrDB = nullptr;

using namespace std;

bool open_kv(KV_BASE_T *kv, const string &name) {
  auto isfull = bool(kv->count());
  if (isfull) {
    if (OPTS.from == MAX_UINT32)
      throw BCException(name + " is not empty. Set -f option properly.");
  } else {
    if (OPTS.from > 0 and OPTS.from != MAX_UINT32)
      throw BCException("-f > 0 but " + name + " k-v is empty. Use '-f 0' or ommit it.");
  }
  return isfull;
}

bool    set_cache(void) {
    if (kv_mode()) {
        string kvtitle;
        switch (OPTS.kvngin) {
          case KVTYPE_KCFILE:
            kvtitle = "Kyotocabinet HashDB";
            TxDB = new KV_KC_DISK_T(OPTS.cachedir / "tx", OPTS.kvtune);
            AddrDB = new KV_KC_DISK_T(OPTS.cachedir / "addr", OPTS.kvtune);
            break;
          case KVTYPE_KCMEM:
            kvtitle = "Kyotocabinet StashDB";
            TxDB = new KV_KC_INMEM_T(OPTS.kvtune);
            AddrDB = new KV_KC_INMEM_T(OPTS.kvtune);
            break;
          case KVTYPE_TKFILE:
            kvtitle = "Tkrzw HashDBM";
            TxDB = new KV_TK_DISK_T(OPTS.cachedir / "tx", OPTS.kvtune);
            AddrDB = new KV_TK_DISK_T(OPTS.cachedir / "addr", OPTS.kvtune);
            break;
          case KVTYPE_TKMEM:
            kvtitle = "Tkrzw TinyDBM";
            TxDB = new KV_TK_INMEM_T(OPTS.kvtune);
            AddrDB = new KV_TK_INMEM_T(OPTS.kvtune);
            break;
          default:
            return b_error("k-v not implemented");
        }
        if (OPTS.verbose)
          cerr << "K-V engine: " << kvtitle << endl;
        auto tx_full = open_kv(TxDB, "tx");
        auto addr_full = open_kv(AddrDB, "addr");
        // TODO: assert tx_full == addr_full
        if (OPTS.from == 0) {
          if (tx_full)
              TxDB->clear();
          if (addr_full)
              AddrDB->clear();
        }
        COUNT.tx = TxDB->count();
        COUNT.addr = AddrDB->count();
    }
    if (OPTS.from == MAX_UINT32)
        OPTS.from = 0;
    return true;
}

void stop_cache(void)
{
  if (kv_mode()) {
      TxDB->close();
      AddrDB->close();
  }
}
