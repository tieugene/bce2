#include <filesystem>
#include <fstream>

#include "bce.h"

using namespace std;

unique_ptr<KV_BASE_T> TxDB = nullptr, AddrDB = nullptr;
static fstream chk_file;

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
            TxDB = make_unique<KV_TK_DISK_T>(OPTS.kvdir, KV_NAME_TX, OPTS.kvtune);
            AddrDB = make_unique<KV_TK_DISK_T>(OPTS.kvdir, KV_NAME_ADDR, OPTS.kvtune);
            break;
          case KVTYPE_TKMEM:
            TxDB = make_unique<KV_TK_INMEM_T>(KV_NAME_TX, OPTS.kvtune);
            AddrDB = make_unique<KV_TK_INMEM_T>(KV_NAME_ADDR, OPTS.kvtune);
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
        if (!TxDB->open())
          return b_error("Cannot open Tx.");
        if (!AddrDB->open()) {
          TxDB->close();
          return b_error("Cannot open Addr.");
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
        // integrity
        auto path = OPTS.kvdir / "bce2.chk";
        chk_file.open(path);
        if(!chk_file.is_open()) {
          chk_file.clear();
          chk_file.open(path, ios::out); //Create file.
          chk_file.close();
          chk_file.open(path);
        }
        if (!chk_file.is_open())
          v_error("Cannot open " + path.string());
    }
    if (OPTS.from == MAX_UINT32)
        OPTS.from = 0;
    return true;
}

void stop_cache(void) {
  if (kv_mode()) {
    TxDB->close();
    AddrDB->close();
    if (chk_file.is_open())
      chk_file.close();
  }
}

bool update_integrity(void) {
  if (chk_file.is_open()) {
    uint32_t data[3] = {COUNT.bk, COUNT.tx, COUNT.addr};
    chk_file.seekp(0);
    chk_file.write((char *) &data, sizeof(data));
  }
  return true;
}
