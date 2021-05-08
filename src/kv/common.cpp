#include <filesystem>

#include "bce.h"
#include "kv.h"

KV_BASE_T  *TxDB = nullptr, *AddrDB = nullptr;

using namespace std;

bool open_kv(KV_BASE_T *kv, const string &name) {
  filesystem::path kvpath = OPTS.cachedir / name;
  if (!kv->init(kvpath))
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
        TxDB = new KV_KC_HASH_T();
        auto tx_full = open_kv(TxDB, "tx");
        AddrDB = new KV_KC_HASH_T();
        auto addr_full = open_kv(AddrDB, "addr");
        if ((tx_full or addr_full) and (OPTS.from == 0)) {
            TxDB->clear();
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
