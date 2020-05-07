/*
Usage: [options] file[ file[...]]
Options:
-f[rom]     - start block
-n[um]      - blocks to process
-q[uiet]    - no output result (stdout)
-v[erbose]  - debug info (stderr)
-c[ache]    - helping data folder
-h[elp]     - subj
*/

#include "misc.h"
#include <filesystem>

namespace fs = std::filesystem;

OPT_T OPTS;
char
    *buffer,    ///< whole of file buffer
    *curptr,    ///< cursor
    *endptr;    ///< EOL
int64_t     bsize;  ///< current block size
uint32_t    bk_counter = 0;
UNIPTR_T    CUR_PTR;
BK_T        CUR_BK;
TX_T        CUR_TX;
VIN_T       CUR_VIN;
VOUT_T      CUR_VOUT;

bool    read_file(string &fn)
{
    ///< Read whole of file into mem
    ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        bsize = file.tellg();   // TODO: handle -1; >128M
        buffer = new char[bsize];
        file.seekg (0, ios::beg);
        file.read (buffer, bsize);
        file.close();
        CUR_PTR.v_ptr = buffer;
        endptr = buffer + bsize;
        return true;
    }
    return false;
}

bool    parse_vin(uint32_t) // FIXME: <= int16
{
    // FIXME: coinbase = 32x00 + 4xFF (txid+vout)
    CUR_VIN.txid = read_256_ptr();
    CUR_VIN.vout = read_32();
    CUR_VIN.ssize = read_v();
    CUR_VIN.script = read_u8_ptr(CUR_VIN.ssize);
    CUR_VIN.seq = read_32();
    if (!OPTS.quiet)
        out_vin();
    if (OPTS.verbose)
        __prn_vin();
    return true;
}

bool    parse_vout(uint32_t no) // FIXME: <= int16
{
    // TODO: out_addr
    CUR_VOUT.no = no;
    CUR_VOUT.satoshi = read_64();
    CUR_VOUT.ssize = read_v();
    CUR_VOUT.script = read_u8_ptr(CUR_VOUT.ssize);
    if (!OPTS.quiet)
        out_vout();
    if (OPTS.verbose)
        __prn_vout();
    return true;
}

bool    parse_tx(uint32_t bk_tx_no) // FIXME: <= int16
{
    CUR_TX.ver = read_32();
    if (!OPTS.quiet)
        out_tx();
    CUR_TX.vins = read_v();
    for (uint32_t i =  0; i < CUR_TX.vins; i++)
        if (!parse_vin(i))
            return false;
    CUR_TX.vouts = read_v();
    for (uint32_t i =  0; i < CUR_TX.vouts; i++)
        if (!parse_vout(i))
            return false;
    CUR_TX.locktime = read_32();
    if (OPTS.verbose)
        __prn_tx();
    return true;
}

bool    parse_bk(uint32_t bk_no)
{
    CUR_BK.head_ptr = static_cast<BK_HEAD_T*> (CUR_PTR.v_ptr);
    if (CUR_BK.head_ptr->sig != 0xD9B4BEF9)    // LE
    {
        cerr << "It's not .dat file: " << hex << CUR_BK.head_ptr->sig << endl;
        return false;
    }
    CUR_PTR.u8_ptr += sizeof (BK_HEAD_T);
    CUR_BK.txs = read_v();
    //mk_hash(CUR_BK.head_ptr, sizeof(BK_HEAD_T), CUR_BK.hash);
    CUR_BK.hash = hash256(CUR_BK.head_ptr, sizeof(BK_HEAD_T));
    if (!OPTS.quiet)
        out_bk();
    if (OPTS.verbose)
        __prn_bk();
    for (uint32_t i = 0; i < CUR_BK.txs; i++, CUR_TX.no++)
        if (!parse_tx(i))
            return false;
    return true;
}

bool    parse_file(void)
{
    for (uint32_t i =  0; i < OPTS.num; i++, CUR_BK.no++)
        if (!parse_bk(i))
            return false;
    return true;
}

int main(int argc, char *argv[])
{
    string  sDatName;

    // -1. handle CLI
    int file1idx = cli(argc, argv);
    if (!file1idx)
        return 1;
    sDatName = OPTS.bkdir + "/" + argv[file1idx];   // FIXME: all of files
    // 0. init
    CUR_BK.no = 0;
    CUR_TX.no = 0;
    // 1. read
    if (!read_file(sDatName))
        return 1;
    if (OPTS.verbose)
        cerr << "File size: " << bsize << endl;
    // 2. parse
    auto result = parse_file();
    if (OPTS.verbose)
        cerr << "File parsing: " << (result ? "OK" : "ERROR!")  << endl;
    // FIXME: delete buffer;
    return 0;
}

//fs::path sDatName = OPTS.bkdir + "/" + argv[file1idx];
//cerr << dir << TAB << file << endl;
//auto sDatPAth = dir.append(file);
//auto sDatPAth = std::filesystem::path(OPTS.bkdir, argv[file1idx]);
