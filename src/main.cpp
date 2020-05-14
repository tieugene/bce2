/*
Usage: [options] file
Options:
-f[rom]     - start block
-n[um]      - blocks to process
-d[atdir]   - *.dat-files folder
-c[ache]    - helping data folder
-q[uiet]    - no output result (stdout)
-v[erbose]  - debug info (stderr)
-k[eep]     - keep existing data
-h[elp]     - subj
*/

#include <filesystem>
#include "bce.h"
#include "misc.h"
#include "script.h"

namespace fs = std::filesystem;

struct  FOFF_T      ///< files-offset array
{
    uint32_t  fileno;
    uint32_t  offset;
};

class   DATFARM_T   ///< represents blk*.dat, opening on demand
{
private:
    ifstream    *file;
    string      folder;
    size_t      qty;
    bool        open(size_t no) {
        if (no > qty) {
            cerr << "File # too big: " << no << endl;
            return false;
        }
        if (file[no].is_open())
            return true;
        ostringstream ss;
        ss << setw(5) << setfill('0') << no;
        string fn = folder + "blk" + ss.str() + ".dat";
        file[no].open(fn, ios::in|ios::binary);
        if (!file[no].is_open()) {
            cerr << "Can't open file " << fn << endl;
            return false;
        }
        return true;
    }
public:
    DATFARM_T(size_t qty, string &folder)
        : folder(folder), qty(qty)
    {
        file = new ifstream[qty];
    }
    bool        read(size_t no, size_t offset, int size, void *dst)
    {
        if (!open(no))
            return false;
        file[no].seekg(offset, file[no].beg);
        file[no].read(static_cast<char *>(dst), size);
        if (file[no].gcount() != size) {
            cerr << "Can't read " << size << " bytes from " << no << "." << offset << endl;
            return false;
        }
        return true;
    }
};

// globals
UNIPTR_T    CUR_PTR;
OPT_T       OPTS;
STAT_T      STAT;
BK_T        CUR_BK;
TX_T        CUR_TX;
VIN_T       CUR_VIN;
VOUT_T      CUR_VOUT;
TxDB_T      TxDB;
BUFFER_T    BUFFER;
// locals
const uint32_t  MAX_BK_SIZE = 2 << 20;  // 2MB enough
const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE
static FOFF_T   *FOFF;
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
    STAT.txs++;
    return true;
}

bool    parse_bk(void)
{
    CUR_BK.head_ptr = static_cast<BK_HEAD_T*> (CUR_PTR.v_ptr);
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
    STAT.max_txs = max(STAT.max_txs, CUR_BK.txs);
    STAT.blocks++;
    return true;
}

bool    load_bk(DATFARM_T &datfarm, uint32_t fileno, uint32_t offset)       ///< load bk to buffer
{
    uint32_t sig, size;
    if (!datfarm.read(fileno, offset-8, sizeof(sig), &sig)) {
        cerr << "Can't read bk signature." << endl;
        return false;
    }
    if (sig != BK_SIGN) {
        cerr << "Block signature not found: " << hex << sig << endl;
        return false;
    }
    if (!datfarm.read(fileno, offset-4, sizeof(size), &size)) {
        cerr << "Can't read bk size." << endl;
        return false;
    }
    if (size > MAX_BK_SIZE) {
        cerr << "Block too big: " << size << endl;
        return false;
    }
    BUFFER.end = BUFFER.beg + size;
    CUR_PTR.v_ptr = BUFFER.beg;
    return datfarm.read(fileno, offset, size, BUFFER.beg);
}

size_t  load_fileoffsets(char *fn)  ///< load file-offset file
{
    ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (!file) {            // 1. open
        cerr << "File '" << fn << "' opening failed" << endl;
        return 0;
    }
    auto data_size = file.tellg();
    if ((data_size < 0) or (data_size & 0x7) or (data_size > (8 << 20)))    // 2. chk filesize
    {
        cerr << "Wrong file size (<0 or != 8x or >8MB (1M bks)): " << fn << "=" << data_size << endl;
        return 0;
    }
    auto blocks = size_t(data_size >> 3);
    FOFF = new FOFF_T[blocks];
    if (!FOFF) {
        cerr << "Can't allocate mem for file-offset list." << endl;
        return 0;
    }
    file.seekg (0, ios::beg);
    char *tmp = static_cast<char *>(static_cast<void *>(FOFF));
    file.read (tmp, data_size);
    file.close();
    return blocks;
}

int     main(int argc, char *argv[])
/* TODO:
 * - local bk_no_upto
 * - local BUFFER
 * [- local FOFF]
 */
{
    // 1. handle CLI
    if (!cli(argc, argv))  // no file defined
        return 1;
    bk_no_upto = OPTS.from + OPTS.num;
    // 1.1. prepare bk info
    auto bk_qty = load_fileoffsets(argv[argc-1]);
    if (!bk_qty)
        return 1;
    if (bk_qty < bk_no_upto) {
        cerr << "Loaded blocks (" << bk_qty << ") < max wanted " << bk_no_upto << endl;
        return 1;
    }
    // 1.2. prepare dat
    if (!OPTS.datdir.empty() and OPTS.datdir.back() != '/')
        OPTS.datdir += '/';  // FIXME: native OS path separator
    DATFARM_T datfarm(bk_qty, OPTS.datdir);
    // 1.3. prepare caches
    if (!OPTS.cachedir.empty() and OPTS.cachedir.back() != '/')
        OPTS.cachedir += '/';  // FIXME: native path separator
    auto s = OPTS.cachedir + "tx.kch";
    if (!TxDB.init(s)) {
        cerr << "Can't open 'TX'tx' cache " << s << endl;
        return 1;
    }
    // 1.3. etc
    BUFFER.beg = new char[MAX_BK_SIZE];
    // 2. main loop
    for (auto i = OPTS.from; i < bk_no_upto; i++)
    {
        CUR_BK.no = i;
        if (load_bk(datfarm, FOFF[i].fileno, FOFF[i].offset))
            parse_bk();
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
