#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include "bce.h"
#include "misc.h"

static string  help_txt = "\
Usage: [options] file[ file[...]]\n\
Options:\n\
-f n      - block starts from (default=0)\n\
-n n      - blocks to process (default=1, 0=all)\n\
-b <path> - blk*.dat folder (default='' - current folder)\n\
-c <path> - cache data folder (default='.')\n\
-q        - quiet (disable output result to stdout)\n\
-v[n]     - verbose (debug info to stderr):\n\
    0 - errors only (default)\n\
    1 - short info (default n)\n\
    2 - mid\n\
    3 - full debug\n\
";

void        __prn_opts(void)
{
    cerr
        << "Options:" << endl
        << TAB << "From:" << TAB << OPTS.from << endl
        << TAB << "Num:" << TAB << OPTS.num << endl
        << TAB << "Quiet:" << TAB << OPTS.quiet << endl
        << TAB << "Debug:" << TAB << OPTS.verbose << endl
        << TAB << "BkDir:" << TAB << OPTS.bkdir << endl
        << TAB << "Cache:" << TAB << OPTS.cache << endl
    ;
}

int         cli(int argc, char *argv[])
{
    int opt;
    int retvalue = 0;

    OPTS.from = 0;
    OPTS.num = 1;
    OPTS.quiet = false;
    OPTS.verbose = 0;
    OPTS.bkdir = "";
    OPTS.cache = ".";
    while ((opt = getopt(argc, argv, "f:n:qv::b:c:")) != -1)
    {
        switch (opt) {
            case 'f':   // FIXME: optarg < 0 | > 999999
                OPTS.from = atoi(optarg);
                break;
            case 'n':   // FIXME: optarg < 1 | > 999999
                //OPTS.num = *optarg == '*' ? 999999 : atoi(optarg);
                OPTS.num = atoi(optarg);
                if (OPTS.num == 0)
                    OPTS.num = 999999;
                break;
            case 'q':
                OPTS.quiet = true;
                break;
            case 'v':   // FIXME: optarg = 0..5
                OPTS.verbose = (optarg) ? atoi(optarg) : 1;
                break;
            case 'b':
                OPTS.bkdir = optarg;
                break;
            case 'c':
                OPTS.cache = optarg;
                break;
            case '?':   // can handle optopt
                cerr << help_txt << endl;
                break;
        }
    }
    // opterr - allways 1
    // optind - 1st file argv's no (if argc > optind)
    if (argc > optind)  {
        retvalue = optind;
        if (OPTS.verbose > 1)   // TODO: up v-level
            __prn_opts();
    }
    else
        cerr << "Error: blk*.dat filename[s] required." << endl << help_txt;
    return retvalue;
}

uint32_t    read_v(void)   ///<read 1..4-byte int and forward;
{
    auto retvalue = static_cast<uint32_t>(*CUR_PTR.u8_ptr++);
    if ((retvalue & 0xFC) == 0xFC) {
        switch (retvalue & 0x03) {
            case 0: // 0xFC
                break;
            case 1: // 0xFD
                retvalue = static_cast<uint32_t>(*CUR_PTR.u16_ptr++);
                break;
            case 2: // 0xFE
                retvalue = *CUR_PTR.u32_ptr++;
                break;
            case 3: // 0xFF
                throw "Value too big";
        }
    }
    return retvalue;
}

uint32_t    read_32(void)  ///< Read 4-byte int and go forward
{
    return *CUR_PTR.u32_ptr++;
}

uint64_t    read_64(void)  ///< Read 8-byte int and go forward
{
    return *CUR_PTR.u64_ptr++;
}

uint8_t     *read_u8_ptr(uint32_t size)
{
    auto retvalue = CUR_PTR.u8_ptr;
    CUR_PTR.u8_ptr += size;
    return retvalue;
}

uint32_t    *read_32_ptr(void)
{
    return CUR_PTR.u32_ptr++;
}

uint256_t   *read_256_ptr(void)
{
    return CUR_PTR.u256_ptr++;
}

string      ptr2hex(void *vptr, size_t size)
{
    static string hex_chars = "0123456789abcdef";
    string s;
    char *cptr = static_cast<char *>(vptr);
    for (size_t i = 0; i < size; i++, cptr++) {
        s.push_back(hex_chars[(*cptr & 0xF0) >> 4]);
        s.push_back(hex_chars[(*cptr & 0x0F)]);
    }
    return s;
}

void        out_vin(void)   // FIXME: compare w/ COINBASE_txid too
{
    if (CUR_VIN.vout != COINBASE_vout)  // skip coinbase
        cout << "i" << TAB << CUR_TX.no << TAB << CUR_VIN.txid << TAB << CUR_VIN.vout << endl;
}

void        out_vout(void)
{
    cout << "o" << TAB << CUR_TX.no << TAB << CUR_VOUT.no << TAB << CUR_VOUT.satoshi << endl;
}

void        out_tx(void)
{
    cout << "t" << TAB << CUR_TX.no << TAB << CUR_BK.no << TAB << hash2hex(CUR_TX.hash) << endl;
}

void        out_bk(void)    ///< Output bk data for DB
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    cout << "b" << TAB << CUR_BK.no << TAB << "'" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << "'" << endl;
}

void        __prn_vin(void)
{
    cerr << "\t\tVin:" << endl;
    if (OPTS.verbose > 2) {
        if (CUR_VIN.vout < 0xFFFFFFFF)
            cerr << "\t\t\tVout_n:\t" << CUR_VIN.vout << endl;
        cerr
            << "\t\t\tSSize:\t" << CUR_VIN.ssize << endl
            << "\t\t\tSeq:\t" << CUR_VIN.seq << endl;
    }
}

void        __prn_vout(void)
{
    cerr
        << "Vout:"
        << TAB << CUR_TX.no
        << " " << CUR_VOUT.no
        << " " << CUR_VOUT.satoshi
        << " " << CUR_VOUT.ssize
        << " " << ptr2hex(CUR_VOUT.script, CUR_VOUT.ssize)
        << endl;
    return;
    cerr << "\t\tVout:" << endl;
    if (OPTS.verbose > 2) {
        cerr
            << "\t\t\t$:\t" << CUR_VOUT.satoshi << endl
            << "\t\t\tSSize:\t" << CUR_VOUT.ssize << endl;
    }
}

void        __prn_tx(void)
{
    cerr
        << "\tTx: " << CUR_TX.no
        << ", hash: " << hash2hex(CUR_TX.hash)
        << ", in: "  << CUR_TX.vins
        << ", out: " << CUR_TX.vouts
        << ", ver: " << CUR_TX.ver
        << ", lock: " << CUR_TX.locktime
        << endl;
    // ver
    //        << "\t\tlock:\t" << CUR_TX.locktime << endl;
}

void        __prn_bk(void)  // TODO: hash
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    cerr
        << "Block: " << CUR_BK.no
        << ", time: " << CUR_BK.head_ptr->time
        << ", hash: " << hash2hex(CUR_BK.hash)
        << ", ver: " << CUR_BK.head_ptr->ver
        << ", txs: " << CUR_BK.txs
        << ", size: " << CUR_BK.head_ptr->size
        << endl;
        // << " (" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")"
}

void        __prn_file(string &fn)
{
    cerr << "File: " << fn
         << ", size: " << (BUFFER.end - BUFFER.beg)
         << ", bk: " << CUR_BK.no
         << ", tx: " << CUR_TX.no
         << endl;
}

void        __prn_summary(void)
{
    cerr << "= Summary =" << endl
        << "Files:" << TAB << STAT.files << endl
        << "Blocks:" << TAB << CUR_BK.no << endl;
    if (OPTS.verbose > 2)
        cerr
            << "Tx:" << TAB << CUR_TX.no << endl
            << "Vins:" << TAB << STAT.vins << endl
            << "Vouts:" << TAB << STAT.vouts << endl
            << "Addrs:" << TAB << STAT.addrs << endl
            << "Vins/tx max:" << TAB << STAT.max_vins << endl
            << "Vouts/tx max:" << TAB << STAT.max_vouts << endl
            << "Addrs/vout max:" << TAB << STAT.max_addrs << endl;
}

// 2.
/*
uint256_t   sha256(void *src, uint32_t size)
{
    uint256_t result;
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src, size);
    SHA256_Final(result.begin(), &context);
    return result;
}

uint256_t   sha256(uint256_t &src)
{
    uint256_t result;
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, src.begin(), src.size());
    SHA256_Final(result.begin(), &context);
    return result;
}

uint256_t   hash256(void *src, uint32_t size)
{
    auto result = sha256(src, size);
    return sha256(result);
}
*/
