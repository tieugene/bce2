#ifndef BCE_H
#define BCE_H

// TODO: stacktrace - file, byte, bk[, tx[, vin+n|vout+n]

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <array>

#include "uintxxx.h"

struct  OPT_T
{
    uint32_t    from = 0;
    uint32_t    num = 1;
    bool        quiet = false;
    int         verbose = 0;
    string      bkdir = "";
    string      cache = "";
};

struct  BUFFER_T
{
    char        *beg = nullptr;
    // char        *cur = nullptr;
    char        *end = nullptr; // end of used space
    uint32_t    size_used = 0;
    uint32_t    size_real = 0;
};

struct  STAT_T
{
    uint32_t    files = 0;
    uint32_t    vins = 0;
    uint32_t    max_vins = 0;
    uint32_t    vouts = 0;
    uint32_t    max_vouts = 0;
    uint32_t    addrs = 0;
    uint32_t    max_addrs = 0;
};

union   UNIPTR_T    ///< Universal ptr
{
    void        *v_ptr;
    uint8_t     *u8_ptr;
    uint16_t    *u16_ptr;
    uint32_t    *u32_ptr;
    uint64_t    *u64_ptr;
    uint256_t   *u256_ptr;
};

struct  BK_HEAD_T   ///< bk header, 88 bytes
{
    uint32_t    sig;
    uint32_t    size;
    uint32_t    ver;
    uint256_t   p_hash;
    uint256_t   mroot;
    uint32_t    time;
    uint32_t    bits;
    uint32_t    nonce;
};

struct  BK_T
{
    uint32_t    no;
    BK_HEAD_T   *head_ptr;
    uint32_t    txs;
    uint256_t   hash;
};

struct  TX_T        ///< transaction variables w/o vins/vouts
{
    uint32_t    no;
    uint32_t    ver;        // FIXME: * (for hash)
    uint32_t    vins;
    uint32_t    vouts;
    uint32_t    locktime;   // FIXME: * (for hash)
    uint256_t   hash;
};

struct  VIN_T
{
    uint256_t   *txid;
    uint32_t    vout;
    uint32_t    ssize;
    uint8_t     *script;
    uint32_t    seq;
};

struct  VOUT_T
{
    uint32_t    no;
    uint64_t    satoshi;
    uint32_t    ssize;
    uint8_t     *script;

};

extern OPT_T    OPTS;
extern STAT_T   STAT;
extern BUFFER_T BUFFER;
extern UNIPTR_T CUR_PTR;
extern BK_T     CUR_BK;
extern TX_T     CUR_TX;
extern VIN_T    CUR_VIN;
extern VOUT_T   CUR_VOUT;

const string TAB = "\t";
const uint32_t COINBASE_vout = 0xFFFFFFFF;

#endif // BCE_H
