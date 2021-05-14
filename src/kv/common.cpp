#include <filesystem>

#include "bce.h"
#include "kv.h"

KV_BASE_T  *TxDB = nullptr, *AddrDB = nullptr;

using namespace std;

bool chk_kv(bool isfull, const string &name) {
  if (isfull) {
    if (OPTS.from == MAX_UINT32)
      return b_error(name + " is not empty. Set -f option properly.");
  } else {
    if (OPTS.from > 0 and OPTS.from != MAX_UINT32)
      return b_error("-f > 0 but " + name + " k-v is empty. Use '-f 0' or ommit it.");
  }
  return true;
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
#ifdef USE_TK
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
#endif
          default:
            return b_error("k-v not implemented");
        }
        if (OPTS.verbose)
          cerr << "K-V engine: " << kvtitle << endl;
        auto tx_full = bool(TxDB->count());
        auto addr_full = bool(AddrDB->count());
        if (!chk_kv(tx_full, "tx") or !chk_kv(addr_full, "addr"))
          return false;
        if (tx_full xor addr_full)
          return b_error("One from tx or addr is empty, another one is full. It is impossible.");
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
