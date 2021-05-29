#ifndef BCE_H
#define BCE_H

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <array>
#include <filesystem>
#include "kv/kv.h"

/// Verbose level
enum    DBG_LVL_T {
  DBG_NONE,
  DBG_MIN,
  DBG_MID,
  DBG_MAX
};

/// Config/CLI options
struct  OPT_T {
  std::filesystem::path  datdir;    // blk*.dat folder path
  std::filesystem::path  locsfile;  // locs-file path
  std::filesystem::path  kvdir;     // file-based k-v dir path
  KVNGIN_T    kvngin = KVTYPE_NONE; // k-v engine
  DBG_LVL_T   verbose = DBG_NONE;   // verbosity
  uint64_t    kvtune = 0;           // k-v tuning
  uint32_t    from = MAX_UINT32;    // bk start from
  uint32_t    num = 1;              // bks to process
  uint32_t    logstep = 1;          // logging step
  bool        fromcin = false;      // input from stdin
  bool        out = false;          // produce output
  bool        mt = false;           // multithreading
};

/// Session counters
struct  STAT_T {
  uint32_t    vins = 0;
  uint32_t    vouts = 0;
  uint32_t    addrs = 0;  // all recognized incl nulldata
  uint32_t    max_txs = 0;
  uint32_t    max_vins = 0;
  uint32_t    max_vouts = 0;
  uint32_t    max_addrs = 0;
  uint32_t    addr_lens[321] = {0}; ///< 0..MAX_ADDR_LEN
};

/// Through counters
struct  COUNT_T {
  uint32_t    bk = 0;
  uint32_t    tx = 0;     // session if no k-v
  uint32_t    addr = 0;   // unique addrs
};

extern OPT_T      OPTS;
extern STAT_T     STAT;
extern COUNT_T    COUNT;
extern KV_BASE_T *TxDB, *AddrDB;

/// Coinbase vin source
const uint32_t COINBASE_vout = 0xFFFFFFFF;

inline bool     kv_mode(void) { return OPTS.kvngin != KVTYPE_NONE; }

#endif // BCE_H
