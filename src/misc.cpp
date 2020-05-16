/*
 * TODO: options:
 * - input bk no=>hash table
 */

#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include "bce.h"
#include "misc.h"
#include "script.h" // cur_addr only

static string  help_txt = "\
Usage: [options] <file-offset_file>\n\
Options:\n\
-f n      - block starts from (default=0)\n\
-n n      - blocks to process (default=1, 0=all)\n\
-d <path> - *.dat folder (default='' - current folder)\n\
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
        << TAB << "DatDir:" << TAB << OPTS.datdir << endl
        << TAB << "Cache:" << TAB << OPTS.cachedir << endl
    ;
}

bool        cli(int argc, char *argv[])
{
    int opt;
    bool retvalue = false;

    OPTS.from = 0;
    OPTS.num = 1;
    OPTS.datdir = "";
    OPTS.cachedir = ".";
    OPTS.quiet = false;
    OPTS.verbose = 0;
    while ((opt = getopt(argc, argv, "f:n:d:c:qv::")) != -1)
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
            case 'd':
                OPTS.datdir = optarg;
                break;
            case 'c':
                OPTS.cachedir = optarg;
                break;
            case 'q':
                OPTS.quiet = true;
                break;
            case 'v':   // FIXME: optarg = 0..5
                OPTS.verbose = (optarg) ? atoi(optarg) : 1;
                break;
            case '?':   // can handle optopt
                cerr << help_txt << endl;
                break;
        }
    }
    // opterr - allways 1
    // optind - 1st file argv's no (if argc > optind)
    if (optind == (argc-1))  {
        retvalue = true;
        if (OPTS.verbose > 1)   // TODO: up v-level
            __prn_opts();
    } else
        cerr << "Error: file-offset filename required." << endl << help_txt;
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
        cout
        << "i" << TAB
        << CUR_TX.no << TAB
        << CUR_VIN.txno << TAB
        << CUR_VIN.vout
        << endl;
        // hash2hex(*CUR_VIN.txid)
}

void        out_vout(void)
{
    cout
        << "o" << TAB
        << CUR_TX.no << TAB
        << CUR_VOUT.no << TAB
        << CUR_VOUT.satoshi
        << endl;
}

void        out_addr(uint32_t id, uint160_t &ripe)
{
    cout
        << "a" << TAB
        << id << TAB
        << ripe2addr(ripe)
        << endl;
}

void        out_xaddr(uint32_t id)
{
    cout
        << "x" << TAB
        << CUR_TX.no << TAB
        << CUR_VOUT.no << TAB
        << id
        << endl;
}

void        out_tx(void)
{
    cout
        << "t" << TAB
        << CUR_TX.no << TAB
        << CUR_BK.no << TAB
        << hash2hex(CUR_TX.hash)
        << endl;
}

void        out_bk(void)    ///< Output bk data for DB
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    cout
        << "b" << TAB
        << CUR_BK.no << TAB
        << "'" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << "'" << TAB
        << hash2hex(CUR_BK.hash)
        << endl;
}

void        __prn_vin(void)
{
    cerr << TAB << "Vin: ";
    if (CUR_VIN.vout == 0xFFFFFFFF)
        cerr << "<coinbase>";
    else
        cerr << " vout: " << CUR_VIN.txno << " " << CUR_VIN.vout;
    cerr
        << ", ssize: " << CUR_VIN.ssize
        << ", seq: " << CUR_VIN.seq
        << endl;
}

void        __prn_vout(void)
{
    cerr
        << TAB << "Vout: "
        << "tx: " << CUR_TX.no
        << ", no: " << CUR_VOUT.no
        << ", $: " << CUR_VOUT.satoshi
        << ", ssize: " << CUR_VOUT.ssize
        << endl;
    //<< " " << ptr2hex(CUR_VOUT.script, CUR_VOUT.ssize)
    return;
    cerr << "\t\tVout:" << endl;
    if (OPTS.verbose > 2) {
        cerr
            << "\t\t\t$:\t" << CUR_VOUT.satoshi << endl
            << "\t\t\tSSize:\t" << CUR_VOUT.ssize << endl;
    }
}

void        __prn_addr(void)
{
    cerr
        << "Addr:"
        << "tx: " << CUR_TX.no
        << ", vout: " << CUR_VOUT.no
        << ", ripe160: " << ripe2addr(CUR_ADDR.addr)
        << endl;
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
        << endl;
        // << " (" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")"
}

void        __prn_summary(void)
{
    cerr << "= Summary =" << endl
        << "Blocks:" << TAB << STAT.blocks << endl;
    if (OPTS.verbose)   // >2
        cerr
            << "Tx:" << TAB << STAT.txs << endl
            << "Vins:" << TAB << STAT.vins << endl
            << "Vouts:" << TAB << STAT.vouts << endl
            << "Addrs:" << TAB << STAT.addrs << endl
            << "Tx/bk max:" << TAB << STAT.max_txs << endl
            << "Vins/tx max:" << TAB << STAT.max_vins << endl
            << "Vouts/tx max:" << TAB << STAT.max_vouts << endl
            << "Addrs/vout max:" << TAB << STAT.max_addrs << endl;
}

bool        __prn_trace(void)
{
    cerr << "*** <Trace> ***" << endl;
    if (CUR_BK.busy)
        cerr << "Block:" << TAB << CUR_BK.no << endl;
    if (CUR_TX.busy)
        cerr << "Tx:" << TAB << CUR_TX.bkno << " (" << CUR_TX.no << ")" << endl;
    if (CUR_VIN.busy)
        cerr << "Vin:" << TAB << CUR_VIN.txno << endl;
    if (CUR_VOUT.busy)
        cerr << "Vout:" << TAB << CUR_VOUT.no << endl;
    cerr << "*** </Trace> ***" << endl;
    return false;
}
