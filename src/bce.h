#ifndef BCE_H
#define BCE_H

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <array>

using namespace std;

typedef array<uint8_t, 32> uint256_t;   ///< hash representation
//typedef uint8_t[32] uint256_t;   ///< hash representation

union UNIPTR_T   ///< Universal ptr
{
    void        *v_ptr;
    uint8_t     *u8_ptr;
    uint16_t    *u16_ptr;
    uint32_t    *u32_ptr;
    uint64_t    *u64_ptr;
    uint256_t   *u256_ptr;
};

struct BK_HEAD_T         ///< bk header, 88 bytes
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

struct TX_T         ///< transaction variables w/o vins/vouts
{
    uint32_t    no;
    uint32_t    ver;
    uint32_t    vins;
    uint32_t    vouts;
    uint32_t    locktime;
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

extern char
    *buffer,    ///< whole of file buffer
    *curptr,    ///< cursor
    *endptr;    ///< EOL
extern int64_t  bsize;  ///< current block size
extern uint32_t bk_counter;
extern UNIPTR_T CUR_PTR;
extern BK_T     CUR_BK;
extern TX_T     CUR_TX;
extern VIN_T    CUR_VIN;
extern VOUT_T   CUR_VOUT;

const string TAB = "\t";
const uint32_t COINBASE_vout = 0xFFFFFFFF;

#endif // BCE_H
