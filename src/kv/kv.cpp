#include <filesystem>
#include <fstream>

#include "bce.h"
#include "out/log.h"

using namespace std;

unique_ptr<KV_BASE_T> TxDB = nullptr, AddrDB = nullptr;
unique_ptr<DBSTAMP_T> StampDB = nullptr;

bool chk_kv(uint32_t count, const string &name) {
  if (count == NOT_FOUND_U32)
    return b_error("Cannot count" + name);
  if (!OPTS.info) {
    if (count) {
      if (OPTS.from == MAX_UINT32)
        return b_error(name + " is not empty. Set -f option properly.");
    } else {
      if (OPTS.from > 0 and OPTS.from != MAX_UINT32)
        return b_error("-f > 0 but " + name + " k-v is empty. Use '-f 0' or ommit it.");
    }
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
        StampDB = make_unique<DBSTAMP_T>(OPTS.kvdir);
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
        if (!StampDB->check())
          return b_error("Integrity check error.");
    }
    if (OPTS.from == MAX_UINT32)
        OPTS.from = 0;
    return true;
}

void stop_cache(void) {
  if (kv_mode()) {
    TxDB->close();
    AddrDB->close();
    StampDB->close();
  }
}

DBSTAMP_T::DBSTAMP_T(const std::filesystem::path &dir) {
  dbpath = dir / "bce2.chk";
  infile = TxDB->infile() and AddrDB->infile();
}

bool DBSTAMP_T::check(void) {
  bool retvalue(true);
  if (infile and OPTS.from) {  // don't check from scratch
    if (filesystem::exists(dbpath)) {
      file.open(dbpath, ios::in | ios::binary);
      if (file.is_open()) {
        if (file.read((char *) data, sizeof(data))) {
          if (OPTS.info) {
            info();
            return true;
          }
          if ((data[0] != OPTS.from) or (data[1] != COUNT.tx) or (data[2] != COUNT.addr))
            retvalue = b_error(
              "Saved: Bk: " + to_string(data[0]) + ", Tx: " + to_string(data[1]) + ", Addr: " + to_string(data[2]) +
              "\nReal:  Bk: " + to_string(OPTS.from) + ", Tx: " + to_string(COUNT.tx) + ", Addr: " + to_string(COUNT.addr) + "\n");
        } else
          retvalue = b_error("Cannot read " + dbpath.string());
        file.close();
      } else
        retvalue = b_error("Cannot open " + dbpath.string() + " to read.");
    }
  }
  return retvalue;
}

bool DBSTAMP_T::update(void) {
  bool retvalue(true);
  if (infile) {
    if(!file.is_open()) {
      file.clear();
      file.open(dbpath, ios::out | ios::binary); //Create file.
    }
    if (file.is_open()) {
      data[0] = COUNT.bk+1;
      data[1] = COUNT.tx;
      data[2] = COUNT.addr;
      file.seekp(0);
      if (!file.write((char *) &data, sizeof(data)))
        retvalue = b_error("Cannot write to file " + dbpath.string());
    } else
      retvalue = b_error("Cannot open " + dbpath.string());
  }
  return retvalue;
}

bool DBSTAMP_T::close(void) {
  bool retvalue(true);
  if (infile) {
    if (file.is_open())
      file.close();
    retvalue = !file.is_open();
  }
  return retvalue;
}
