#ifndef BCE_H
#define BCE_H

// TODO: replace OPTS.cashe+inmem with file || inmem
// TODO: stacktrace - file, byte, bk[, tx[, vin+n|vout+n]

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <array>
#include <filesystem>
#include "kv.h"

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
    uint64_t    kvtune = 0;           // k-v tuning
    uint32_t    from = MAX_UINT32;    // bk start from
    uint32_t    num = 1;              // bks to process
    bool        fromcin = false;      // input from stdin
    bool        out = false;          // produce output
    DBG_LVL_T   verbose = DBG_NONE;   // verbosity
    uint64_t    logstep = 1;          // logging step
};

/// Through counters
struct  COUNT_T {
    uint32_t    bk = 0;
    uint32_t    tx = 0;     // session if no cache
    uint32_t    addr = 0;
};

/// Session counters
struct  STAT_T {
    uint32_t    max_txs = 0;
    uint32_t    vins = 0;
    uint32_t    max_vins = 0;
    uint32_t    vouts = 0;
    uint32_t    max_vouts = 0;
    uint32_t    addrs = 0;
    uint32_t    max_addrs = 0;
};

/// Per parent counters
struct  LOCAL_T {
    uint32_t    tx = 0;
    uint32_t    vin = 0;
    uint32_t    vout = 0;
    uint32_t    wit = 0;
};

/// 'Item is processing' flag (for trace)
struct  BUSY_T {
    bool    bk = false;
    bool    tx = false;
    bool    vin = false;
    bool    vout = false;
};

/// Bk header (80 bytes)
struct  BK_HEAD_T {
    uint32_t    ver;    // real head start (80 bytes)
    uint256_t   p_hash;
    uint256_t   mroot;
    uint32_t    time;
    uint32_t    bits;
    uint32_t    nonce;  // real head end
};

/// Whole of bk data w/o txs
struct  BK_T {
    const BK_HEAD_T   *head_ptr;
    uint32_t    txs;
    uint256_t   hash;
};

/// Tx variables w/o vins/vouts
struct  TX_T {
    uint32_t    ver;        // FIXME: * (for hash)
    uint32_t    vins;
    uint32_t    vouts;
    //uint32_t    wits;
    uint32_t    locktime;   // FIXME: * (for hash)
    uint256_t   hash;
    bool        segwit;
};

/// Vin data
struct  VIN_T {
    const uint256_t   *txid;
    uint64_t    txno;       // ?
    uint32_t    vout;
    uint32_t    ssize;      // vint
    const u8_t *script;
    uint32_t    seq;
};

/// Vout data
struct  VOUT_T {
    uint64_t    satoshi;
    uint32_t    ssize;      // vint
    const u8_t *script;
};

extern OPT_T    OPTS;
extern DBG_LVL_T DBG_LVL;
extern COUNT_T  COUNT;
extern STAT_T   STAT;
extern LOCAL_T  LOCAL;
extern BUSY_T   BUSY;
extern BK_T     CUR_BK;
extern TX_T     CUR_TX;
extern VIN_T    CUR_VIN;
extern VOUT_T   CUR_VOUT;
extern UNIPTR_T CUR_PTR;
extern KV_BASE_T     *TxDB, *AddrDB;
extern time_t   start_time;
extern long     start_mem;

const std::string TAB = "\t";
/// Coinbase vin source
const uint32_t COINBASE_vout = 0xFFFFFFFF;

inline bool     kv_mode(void) { return OPTS.kvngin != KVTYPE_NONE; }

#endif // BCE_H
