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
BUFFER_T    BUFFER;
#ifdef MEM
TxMAP_T    TxDB;
AddrMAP_T  AddrDB;
#else
TxDB_T    TxDB;
AddrDB_T  AddrDB;
#endif
// locals
static uint32_t BK_GLITCH[] = {91722, 91842};    // dup 91880, 91812
static FOFF_T   *FOFF;
// consts
const uint32_t  MAX_BK_SIZE = 2 << 20;  // 2MB enough
const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE
const uint32_t    BULK_SIZE = 10000;
// forwards
size_t  load_fileoffsets(char *fn);  ///< load file-offset file
bool    load_bk(DATFARM_T &datfarm, uint32_t fileno, uint32_t offset);       ///< load bk to buffer
bool    parse_bk(void);
bool    parse_tx(uint32_t bk_tx_no); // TODO: hash
bool    parse_vin(uint32_t no);
bool    parse_vout(uint32_t no);
bool    parse_script(void);

int     main(int argc, char *argv[])
/* TODO:
 * - local BUFFER
 * [- local FOFF]
 */
{
    // 1. handle CLI
    if (!cli(argc, argv))  // no file defined
        return 1;
    auto bk_no_upto = OPTS.from + OPTS.num;
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
        cerr << "Can't open 'tx' cache: " << s << endl;
        return 1;
    }
    s = OPTS.cachedir + "addr.kch";
    if (!AddrDB.init(s)) {
        cerr << "Can't open 'addr' cache " << s << endl;
        return 1;
    }
    // 1.3. etc
    BUFFER.beg = new char[MAX_BK_SIZE];
    // 2. main loop
    for (auto i = OPTS.from; i < bk_no_upto; i++)
    {
        CUR_TX.busy = CUR_VIN.busy = CUR_VOUT.busy = false;
        CUR_BK.busy = true;
        CUR_BK.no = i;
        if (!load_bk(datfarm, FOFF[i].fileno, FOFF[i].offset))
            break;
        if (!parse_bk()) {
            __prn_trace();
            break;
        }
        if ((OPTS.verbose > 3) and ((i % BULK_SIZE) == 0))
            cerr << i << endl;
        CUR_TX.busy = CUR_VIN.busy = CUR_VOUT.busy = false;
        CUR_BK.busy = false;
    }
    // x. The end
    if (OPTS.verbose)
        __prn_summary();
    if (BUFFER.beg)
        delete BUFFER.beg;
    return 0;
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
    //BUFFER.end = BUFFER.beg + size;
    CUR_PTR.v_ptr = BUFFER.beg;
    return datfarm.read(fileno, offset, size, BUFFER.beg);
}

bool    parse_bk(void)
{

    CUR_BK.head_ptr = static_cast<BK_HEAD_T*> (CUR_PTR.v_ptr);
    CUR_PTR.u8_ptr += sizeof (BK_HEAD_T);
    CUR_BK.txs = read_v();
    if (!OPTS.quiet or OPTS.verbose >= 2) // on demand
        hash256(CUR_BK.head_ptr, sizeof(BK_HEAD_T), CUR_BK.hash);
    if (!OPTS.quiet)
        out_bk();
    if (OPTS.verbose >= 2)
        __prn_bk();
    if (!(CUR_BK.no == BK_GLITCH[0] or CUR_BK.no == BK_GLITCH[1]))
        for (uint32_t i = 0; i < CUR_BK.txs; i++, CUR_TX.no++)
            if (!parse_tx(i))
                return false;
    STAT.max_txs = max(STAT.max_txs, CUR_BK.txs);
    STAT.blocks++;
    return true;
}

bool    parse_tx(uint32_t bk_tx_no) // TODO: hash
{
    CUR_TX.busy = true;
    auto h_beg = CUR_PTR.u8_ptr;
    CUR_TX.bkno = bk_tx_no;
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
    auto tx_added = TxDB.add(CUR_TX.hash);
    if (tx_added == NOT_FOUND_U32) {
            cerr << "Can't add tx " << hash2hex(CUR_TX.hash) << endl;
            return false;
    }
    if (tx_added != CUR_TX.no) {
            cerr << "Added tx " << hash2hex(CUR_TX.hash) << " added as " << tx_added << " against waiting " << CUR_TX.no << endl;
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
    CUR_TX.busy = false;
    return true;
}

bool    parse_vin(uint32_t no)
{
    // FIXME: coinbase = 32x00 + 4xFF (txid+vout)
    CUR_VIN.busy = true;
    CUR_VIN.no = no;
    CUR_VIN.txid = read_256_ptr();
    CUR_VIN.vout = read_32();
    CUR_VIN.ssize = read_v();
    CUR_VIN.script = read_u8_ptr(CUR_VIN.ssize);
    CUR_VIN.seq = read_32();
    if (CUR_VIN.vout != COINBASE_vout) {
        CUR_VIN.txno = TxDB.get(*CUR_VIN.txid);
        if (CUR_VIN.txno == NOT_FOUND_U32) {
            cerr << "txid " << hash2hex(*CUR_VIN.txid) << " not found." << endl;
            return false;
        }
    }
    if (!OPTS.quiet)
        out_vin();
    if (OPTS.verbose >= 4)
        __prn_vin();
    CUR_VIN.busy = false;
    return true;
}

bool    parse_vout(uint32_t no)
{
    // TODO: out_addr
    CUR_VOUT.busy = true;
    CUR_VOUT.no = no;
    CUR_VOUT.satoshi = read_64();
    CUR_VOUT.ssize = read_v();
    CUR_VOUT.script = read_u8_ptr(CUR_VOUT.ssize);
    if (!OPTS.quiet)
        out_vout();
    if (OPTS.verbose >= 4)
        __prn_vout();
    //if (!parse_script())
    //    return false;
    CUR_VOUT.busy = false;
    return true;
}


bool    parse_script(void)
{
    if (!script_decode(CUR_VOUT.script, CUR_VOUT.ssize))
        return false;    // !!! TERMPORARY !!!
    auto addr_id = AddrDB.get(CUR_ADDR.addr);
    if (addr_id == NOT_FOUND_U32) {
        addr_id = AddrDB.add(CUR_ADDR.addr);
        if (addr_id == NOT_FOUND_U32) {
            cerr << "Can not find nor add addr " << ripe2hex(CUR_ADDR.addr) << endl;
            return false;
        }
        if (!OPTS.quiet)
            out_addr(addr_id, CUR_ADDR.addr);
        STAT.addrs += 1;
    }
    if (!OPTS.quiet)
        out_xaddr(addr_id);
    if (OPTS.verbose >= 4)
        __prn_addr();
    return true;
}

//fs::path sDatName = OPTS.bkdir + "/" + argv[file1idx];
//cerr << dir << TAB << file << endl;
//auto sDatPAth = dir.append(file);
//auto sDatPAth = std::filesystem::path(OPTS.bkdir, argv[file1idx]);
