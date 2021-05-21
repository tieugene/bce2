#include <filesystem>

#include "bce.h"
#include "kv/kv.h"

KV_BASE_T  *TxDB = nullptr, *AddrDB = nullptr;

using namespace std;

bool chk_kv(uint32_t count, const string &name) {
  if (count == NOT_FOUND_U32)
    return b_error("Cannot count" + name);
  if (count) {
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
        switch (OPTS.kvngin) {
#ifdef USE_KC
          case KVTYPE_KCFILE:
            TxDB = new KV_KC_DISK_T(OPTS.kvdir, KV_NAME_TX, OPTS.kvtune);
            AddrDB = new KV_KC_DISK_T(OPTS.kvdir, KV_NAME_ADDR, OPTS.kvtune);
            break;
          case KVTYPE_KCMEM:
            TxDB = new KV_KC_INMEM_T(KV_NAME_TX, OPTS.kvtune);
            AddrDB = new KV_KC_INMEM_T(KV_NAME_ADDR, OPTS.kvtune);
            break;
#endif
#ifdef USE_TK
          case KVTYPE_TKFILE:
            TxDB = new KV_TK_DISK_T(OPTS.kvdir, KV_NAME_TX, OPTS.kvtune);
            AddrDB = new KV_TK_DISK_T(OPTS.kvdir, KV_NAME_ADDR, OPTS.kvtune);
            break;
          case KVTYPE_TKMEM:
            TxDB = new KV_TK_INMEM_T(KV_NAME_TX, OPTS.kvtune);
            AddrDB = new KV_TK_INMEM_T(KV_NAME_ADDR, OPTS.kvtune);
            break;
#endif
#ifdef USE_BDB
          case KVTYPE_BDB:
            TxDB = new KV_BDB_T(OPTS.kvdir, KV_NAME_TX, OPTS.kvtune);
            AddrDB = new KV_BDB_T(OPTS.kvdir, KV_NAME_ADDR, OPTS.kvtune);
            break;
#endif
          default:
            return b_error("k-v not implemented");
        }
        auto tx_count = TxDB->count();
        auto addr_count = AddrDB->count();
        if (!chk_kv(tx_count, "tx") or !chk_kv(addr_count, "addr"))
          return false;
        if (bool(tx_count) xor bool(addr_count))
          return b_error("One from tx or addr is empty, another one is full. It is impossible.");
        if (OPTS.from == 0) {
          if (tx_count)
            TxDB->clear();
          if (addr_count)
            AddrDB->clear();
        }
        COUNT.tx = TxDB->count();
        if (COUNT.tx == NOT_FOUND_U32)
          return b_error("Cannot count tx #2");
        COUNT.addr = AddrDB->count();
        if (COUNT.addr == NOT_FOUND_U32)
          return b_error("Cannot count addr #2");
    }
    if (OPTS.from == MAX_UINT32)
        OPTS.from = 0;
    return true;
}

void stop_cache(void)
{
  if (kv_mode()) {
      delete TxDB;
      delete AddrDB;
  }
}
