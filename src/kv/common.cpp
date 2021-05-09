#include <filesystem>

#include "bce.h"
#include "kv.h"

KV_BASE_T  *TxDB = nullptr, *AddrDB = nullptr;

using namespace std;

bool open_kv(KV_BASE_T *kv, const string &name) {
  filesystem::path kvpath = OPTS.cachedir / name;
  if (!kv->init(kvpath, OPTS.kvtune))
    throw "Cannot init k-v " + name;
  auto isfull = bool(kv->count());
  if (!isfull and OPTS.from > 0)
    throw "-f > 0 but " + name + " k-v is empty. Use '-f 0' to clean.";
  if (isfull and OPTS.from == MAX_UINT32)
    throw name + " is not empty. Set -f option properly.";
  return isfull;
}

bool    set_cache(void) {
    if (OPTS.cash) {
        string kvtitle;
        if (OPTS.kvngin == "kcf") {
          kvtitle = "Kyotocabinet HashDB";
          TxDB = new KV_KC_DISK_T();
          AddrDB = new KV_KC_DISK_T();
        } else if (OPTS.kvngin == "kcm") {
          kvtitle = "Kyotocabinet StashDB";
          TxDB = new KV_KC_INMEM_T();
          AddrDB = new KV_KC_INMEM_T();
        } else if (OPTS.kvngin == "tkf") {
          kvtitle = "Tkrzw HashDBM";
          TxDB = new KV_TK_DISK_T();
          AddrDB = new KV_TK_DISK_T();
          } else if (OPTS.kvngin == "tkm") {
            kvtitle = "Tkrzw TinyDBM";
            TxDB = new KV_TK_INMEM_T();
            AddrDB = new KV_TK_INMEM_T();
        } else {
          cerr << OPTS.kvngin << " not implemented" << endl;
          return false;
        }
        if (OPTS.verbose)
          cerr << "K-V engine: " << kvtitle << endl;
        auto tx_full = open_kv(TxDB, "tx");
        auto addr_full = open_kv(AddrDB, "addr");
        if (OPTS.from == 0) {
          if (tx_full)
              TxDB->clear();
          if (addr_full)
              AddrDB->clear();
        }
        COUNT.tx = TxDB->count();
        COUNT.addr = AddrDB->count();
    } else    // not k-v mode
        if (OPTS.from == MAX_UINT32)
            OPTS.from = 0;
    return true;
}

void stop_cache(void)
{
  if (OPTS.cash) {
      TxDB->close();
      AddrDB->close();
  }
}
