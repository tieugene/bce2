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

#include <filesystem>
#include "bce.h"
#include "misc.h"
#include "script.h"

namespace fs = std::filesystem;

OPT_T       OPTS;
BUFFER_T    BUFFER;
STAT_T      STAT;
UNIPTR_T    CUR_PTR;
BK_T        CUR_BK;
TX_T        CUR_TX;
VIN_T       CUR_VIN;
VOUT_T      CUR_VOUT;
TxDB_T      TxDB;

static uint32_t bk_no_upto;

bool    parse_vin(uint32_t no)
{
    // FIXME: coinbase = 32x00 + 4xFF (txid+vout)
    CUR_VIN.txid = read_256_ptr();
    CUR_VIN.vout = read_32();
    CUR_VIN.ssize = read_v();
    CUR_VIN.script = read_u8_ptr(CUR_VIN.ssize);
    CUR_VIN.seq = read_32();
    if (CUR_VIN.vout != COINBASE_vout) {
        CUR_VIN.txno = TxDB.get(*CUR_VIN.txid);
        if (CUR_VIN.txno == NOT_FOUND_U32) {
            cerr << "txid " << hash2hex(*CUR_VIN.txid) << " not foud." << endl;
            return false;
        }
    }
    if (!OPTS.quiet)
        out_vin();
    if (OPTS.verbose >= 4)
        __prn_vin();
    return true;
}

bool    parse_vout(uint32_t no)
{
    // TODO: out_addr
    CUR_VOUT.no = no;
    CUR_VOUT.satoshi = read_64();
    CUR_VOUT.ssize = read_v();
    CUR_VOUT.script = read_u8_ptr(CUR_VOUT.ssize);
    auto addr_qty = script_decode(CUR_VOUT.script, CUR_VOUT.ssize);
    if (addr_qty != 1)  // dirty hack
        return false;
    if (!OPTS.quiet) {
        out_vout();
        out_addr();
    }
    if (OPTS.verbose >= 4) {  // FIXME: >= 4; debug - 1
        __prn_vout();
        __prn_addr();
    }
    STAT.addrs += 1;    // dirty_hack
    return true;
}

bool    parse_tx(uint32_t bk_tx_no) // TODO: hash
{
    auto h_beg = CUR_PTR.u8_ptr;
    CUR_TX.ver = read_32();
    CUR_TX.vins = read_v();
    for (uint32_t i =  0; i < CUR_TX.vins; i++)
        if (!parse_vin(i))
            return false;
    CUR_TX.vouts = read_v();
    for (uint32_t i =  0; i < CUR_TX.vouts; i++)
        if (!parse_vout(i))
            return false;
    CUR_TX.locktime = read_32();
    auto h_end = CUR_PTR.u8_ptr;
    hash256(h_beg, h_end - h_beg, CUR_TX.hash);
    if (!TxDB.add(CUR_TX.hash, CUR_TX.no)) {
            cerr << "Can't add tx " << hash2hex(CUR_TX.hash) << endl;
            return false;
    }
    if (!OPTS.quiet)
        out_tx();
    if (OPTS.verbose >= 3)
        __prn_tx();
    STAT.vins += CUR_TX.vins;
    STAT.vouts += CUR_TX.vouts;
    STAT.max_vins = max(STAT.max_vins, CUR_TX.vins);
    STAT.max_vouts = max(STAT.max_vouts, CUR_TX.vouts);
    return true;
}

bool    parse_bk(bool skip)
{
    CUR_BK.head_ptr = static_cast<BK_HEAD_T*> (CUR_PTR.v_ptr);
    if (CUR_BK.head_ptr->sig != 0xD9B4BEF9)    // LE
    {
        cerr << "Block signature not found: " << hex << CUR_BK.head_ptr->sig << endl;
        return false;
    }
    if (skip) {
        if (OPTS.verbose >= 3)
            cerr << "Block: " << CUR_BK.no << " <skipped>" << endl;
        CUR_PTR.u8_ptr += (CUR_BK.head_ptr->size + 8);  // including sig and size
        return true;
    }
    CUR_PTR.u8_ptr += sizeof (BK_HEAD_T);
    CUR_BK.txs = read_v();
    if (!OPTS.quiet or OPTS.verbose >= 2) // on demand
        hash256(&(CUR_BK.head_ptr->ver), sizeof(BK_HEAD_T)-8, CUR_BK.hash);
    if (!OPTS.quiet)
        out_bk();
    if (OPTS.verbose >= 2)
        __prn_bk();
    for (uint32_t i = 0; i < CUR_BK.txs; i++, CUR_TX.no++)
        if (!parse_tx(i))
            return false;
    return true;
}

bool    parse_file(void)    // TODO: err | EOF | -n exceeded
{
    while (CUR_BK.no < bk_no_upto and CUR_PTR.v_ptr < BUFFER.end) {
        bool skip = (CUR_BK.no < OPTS.from);
        if (!parse_bk(skip))
            return false;
        CUR_BK.no++;
    }
    return true;
}

bool    read_file(string &fn)
{
    // TODO: check path exists, is file
    ///< Read whole of file into mem
    ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (!file) {
        cerr << "File '" << fn << "' opening failed" << endl;
        return false;
    }
    auto data_size = file.tellg();
    if (data_size < 0) {
        cerr << "Can't get size of '" << fn << "'" << endl;
        return false;
    }
    auto udata_size = uint32_t(data_size);
    if (OPTS.verbose > 1)
        cerr << fn << " ("<< udata_size << " bytes):" << endl;
    if (BUFFER.size_real < udata_size) {
        if (BUFFER.beg)
            if (OPTS.verbose > 1)
                cerr << "Reallocating buffer." << endl;
            delete BUFFER.beg;
        BUFFER.beg = new char[udata_size];
        BUFFER.size_real = udata_size;
    }
    file.seekg (0, ios::beg);
    file.read (BUFFER.beg, udata_size);
    file.close();
    BUFFER.size_used = udata_size;
    BUFFER.end = BUFFER.beg + udata_size;
    CUR_PTR.v_ptr = BUFFER.beg;
    return true;
}

int     main(int argc, char *argv[])
{
    // 1. handle CLI
    int file1idx = cli(argc, argv);
    if (!file1idx)  // no file[s] defined
        return 1;
    // 1.1. prepare dat
    if (!OPTS.bkdir.empty() and OPTS.bkdir.back() != '/')   // ?'!'
        OPTS.bkdir += '/';  // FIXME: path separator
    // 1.2. prepare cache
    if (!OPTS.cache.empty() and OPTS.cache.back() != '/')
        OPTS.cache += '/';  // FIXME: path separator
    auto s = OPTS.cache + "tx.kch";
    if (!TxDB.init(s)) {
        cerr << "Can't open cache " << s << endl;
    }
    // 1.3. etc
    bk_no_upto = OPTS.from + OPTS.num;
    // 2. main loop
    for (auto i = file1idx; i < argc; i++)
    {
        auto sDatName = OPTS.bkdir + argv[i];   // TODO: merge path
        // 2. read
        if (!read_file(sDatName))
            break;
        // 3. parse
        auto result = parse_file();
        STAT.files++;
        if (not result) {
            cerr << "Error in file '" << sDatName << "' offset " << hex << (static_cast<char *>(CUR_PTR.v_ptr) - BUFFER.beg) << endl;
            break;
        }
        if (OPTS.verbose)
            __prn_file(sDatName);
            //cerr << "File parsing: OK" << endl;
        if (CUR_BK.no >= bk_no_upto)    // -n exceeded
            break;
    }
    // x. The end
    if (OPTS.verbose)
        __prn_summary();
    if (BUFFER.beg)
        delete BUFFER.beg;
    return 0;
}

//fs::path sDatName = OPTS.bkdir + "/" + argv[file1idx];
//cerr << dir << TAB << file << endl;
//auto sDatPAth = dir.append(file);
//auto sDatPAth = std::filesystem::path(OPTS.bkdir, argv[file1idx]);
