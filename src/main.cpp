/*
need - params, bc container
commands:
- info/check
- export
params (export):
-f[rom] block
-q[ty] blocks
-k[eep]
intermediates:
- tx hashes
- address hashes
== each block:
- assign sig+len+header
- get txins; for each:
-- assign ver
-- get vcount; for each:
--- assign txid
--- assign vout
--- get ssize
--- assign script
--- assign seq
-- get vcount; for each:
--- assign satoshi
--- get ssize
--- assign script
-- assign locktime
== find:
- bk height - check by p_hash?
- calc block hash (2 x sha256 of bk.ver..bk.nonce included; print in reverse)
- ~~calc tx hash~~ (2 x sha256 of tx.ver..tx.locktime included; print in reverse)
*/

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <array>

using namespace std;

typedef array<uint8_t, 32> uint256_t;   ///< hash representation

static string sBCDir = "/mnt/shares/home/eugene/ftp/VCS/my/GIT/bce";   //"/mnt/sdb2/bitcoin/blocks";
static char
    *buffer,    ///< whole of file buffer
    *curptr,    ///< cursor
    *endptr;    ///< EOL
static int64_t bsize;  ///< current block size
static uint32_t bk_counter = 0;

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

// TODO: BK_t

union UNIPTR_T   ///< Universal ptr
{
    void        *v_ptr;
    uint8_t     *u8_ptr;
    uint16_t    *u16_ptr;
    uint32_t    *u32_ptr;
    uint64_t    *u64_ptr;
    uint256_t   *u256_ptr;
};

struct TX_T         ///< transaction variables w/o vins/vouts
{
    uint32_t    ver;
    uint64_t    vins;
    uint64_t    vouts;
    uint32_t    locktime;
};

struct  TX_IN_T
{
    uint256_t   *txid;
    uint32_t    vout;
    uint64_t    ssize;
    uint8_t     *script;
    uint32_t    seq;
};

struct  TX_OUT_T
{
    uint64_t    satoshi;
    uint64_t    ssize;
    uint8_t     *script;
};

static  UNIPTR_T PTR;

bool read_file(string &fn)
{
    ///< Read whole of file into mem
    ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        bsize = file.tellg();   // TODO: handle -1
        buffer = new char[bsize];
        file.seekg (0, ios::beg);
        file.read (buffer, bsize);
        file.close();
        PTR.v_ptr = buffer;
        endptr = buffer + bsize;
        return true;
    }
    return false;
}

uint64_t read_v(void)   ///<read 1..4-byte int and forward;
{
    auto retvalue = static_cast<uint64_t>(*PTR.u8_ptr++);
    if ((retvalue & 0xFC) == 0xFC) {
        switch (retvalue & 0x03) {
            case 0: // 0xFC
                break;
            case 1: // 0xFD
                retvalue = static_cast<uint16_t>(*PTR.u16_ptr++);
                break;
            case 2: // 0xFE
                retvalue = static_cast<uint32_t>(*PTR.u32_ptr++);
                break;
            case 3: // 0xFF
                retvalue = static_cast<uint64_t>(*PTR.u64_ptr++);
                break;
        }
    }
    return retvalue;
}

uint32_t read_32(void)  ///< Read 4-byte int and go forward
{
    return *PTR.u32_ptr++;
}

uint64_t read_64(void)  ///< Read 8-byte int and go forward
{
    return *PTR.u64_ptr++;
}

uint256_t *read_256_ptr(void)
{
    return PTR.u256_ptr++;
}

uint8_t *read_u8_ptr(uint64_t size)
{
    auto retvalue = PTR.u8_ptr;
    PTR.u8_ptr += size;
    return retvalue;
}

void    __prn_vin(TX_IN_T &vin)
{
    cerr << "\t\tVin:" << endl
         << "\t\t\tVout_n:\t" << vin.vout << endl
         << "\t\t\tSSize:\t" << vin.ssize << endl
         << "\t\t\tSeq:\t" << vin.seq << endl
         ;
}

void    __prn_vout(TX_OUT_T &vout)
{
    cerr << "\t\tVout:" << endl
         << "\t\t\t$:\t" << vout.satoshi << endl
         << "\t\t\tSSize:\t" << vout.ssize << endl
         ;
}

void    __prn_tx(TX_T &tx)
{
    cerr << "\tTx:" << endl
         << "\t\tVer:\t" << tx.ver << endl
         << "\t\tvins:\t" << tx.vins << endl
         << "\t\tvouts:\t" << tx.vouts << endl
         << "\t\tlock:\t" << tx.locktime << endl
         ;
}

void    __prn_bk(BK_HEAD_T *bk_h_ptr, uint32_t no)
{
    time_t t = bk_h_ptr->time;
    cerr    << "Block: " << no << endl
            << "\tSize:\t" << bk_h_ptr->size << endl
            << "\tVer:\t" << bk_h_ptr->ver << endl
            << "\tTime:\t" << bk_h_ptr->time << " = "
            << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << endl
            ;
}

bool    parse_vin(uint64_t) // FIXME: <= int16
{
    // FIXME: coinbase = 32x00 + 4xFF (txid+vout)
    TX_IN_T vin;
    vin.txid = read_256_ptr();
    vin.vout = read_32();
    vin.ssize = read_v();
    vin.script = read_u8_ptr(vin.ssize);
    vin.seq = read_32();
    __prn_vin(vin);
    // TODO: out_vint()
    return true;
}

bool    parse_vout(uint64_t) // FIXME: <= int16
{
    TX_OUT_T vout;
    vout.satoshi = read_64();
    vout.ssize = read_v();
    vout.script = read_u8_ptr(vout.ssize);
    __prn_vout(vout);
    // TODO: out_vout() => out_addr
    return true;
}

bool    parse_tx(uint64_t bk_tx_no) // FIXME: <= int16
{
    TX_T tx;
    tx.ver = read_32();
    // TODO: out_tx()
    tx.vins = read_v();
    for (uint64_t i =  0; i < tx.vins; i++)
        if (!parse_vin(i))
            return false;
    tx.vouts = read_v();
    for (uint64_t i =  0; i < tx.vouts; i++)
        if (!parse_vout(i))
            return false;
    tx.locktime = read_32();
    __prn_tx(tx);
    return true;
}

void    out_bk(BK_HEAD_T *bk_h_ptr)    ///< Output bk data for DB
{
    time_t t = bk_h_ptr->time;
    //cout << bk_counter << "\t" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << endl;
}

bool    parse_bk(uint32_t bk_no)
{
    BK_HEAD_T *bk_h_ptr;
    bk_h_ptr = static_cast<BK_HEAD_T*> (PTR.v_ptr);
    if (bk_h_ptr->sig != 0xD9B4BEF9)    // LE
    {
        cerr << "It's not .dat file: " << hex << bk_h_ptr->sig << endl;
        return false;
    }
    PTR.u8_ptr += sizeof (BK_HEAD_T);
    __prn_bk(bk_h_ptr, bk_no);
    out_bk(bk_h_ptr);
    auto tx_count = read_v();
    cerr << "\tTxs: " << tx_count << endl;
    //for (auto i = 0; i < tx_count; i++) {
    for (uint64_t i =  0; i < tx_count; i++)
        if (!parse_tx(i))
            return false;
    return true;
}

bool    parse_file(void)
{
    //return parse_bk();
    for (uint64_t i =  0; i < 10000; i++)
        if (!parse_bk(i))
            return false;
    return true;
}

int main(int argc, char *argv[])
{
    auto sDatName = sBCDir + "/blk00000.dat";
    // 1. read
    if (!read_file(sDatName))
            return 1;
    cerr << "File size: " << bsize << endl;
    // 2. parse
    auto result = parse_file();
    cerr << "File parsing: " << (result ? "OK" : "ERROR!")  << endl;
    //cerr << "BK header size: " << sizeof(BK_HEAD_T) << endl;
    // FIXME: delete buffer;
    return 0;
}
