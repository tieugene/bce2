/* printer.cpp
 * Program outputs
 */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <time.h>
#include "bce.h"
#include "misc.h"
#include "printers.h"
#include "script.h"

using namespace std;

// Uncomment to use bce2 as addresses generator only
// #define DBG

void        out_bk(void)    ///< Output bk data for DB
{   // FIXME: hash can be w/o 's
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    char dt[20];
    strftime(dt, 20, "%Y-%m-%d %OH:%OM:%OS", localtime(&t));   ///FUTURE: back into gmtime
    printf("b\t%u\t'%s'\t'%s'\n", COUNT.bk, dt, hash2hex(CUR_BK.hash).c_str()); ///FUTURE: s/'hash'/hash/
}

void        out_tx(void)
{
  printf("t\t%u\t%u\t%s\n", COUNT.tx, COUNT.bk, hash2hex(CUR_TX.hash).c_str());
}

void        out_vin(void)   // FIXME: compare w/ COINBASE_txid too
{
    if (CUR_VIN.vout != COINBASE_vout)  // skip coinbase
        printf("i\t%" PRIu64 "\t%u\t%u\n", CUR_VIN.txno, CUR_VIN.vout, COUNT.tx);
}

void        out_vout(void)
{
  if (CUR_ADDR.get_qty())
    printf("o\t%u\t%u\t%" PRIu64 "\t%u\n", COUNT.tx, LOCAL.vout, CUR_VOUT.satoshi, CUR_ADDR.get_id());
  else
    printf("o\t%u\t%u\t%" PRIu64 "\t\\N\n", COUNT.tx, LOCAL.vout, CUR_VOUT.satoshi);
}

void        out_addr(void)
{   // single:      "symbols"
    // multisign:   ["one", "two"]
    auto alist = CUR_ADDR.get_strings();
    string v;
    if (alist.size()) {
        if (alist.size() == 1)
            v = "\"" + alist[0] + "\"";
        else {
            v = "[\"" + alist[0];
            for (size_t i = 1; i < alist.size(); i++)
                v = v + "\", \"" + alist[i];
            v += "\"]";
        }
    }
    printf("a\t%u\t%s\t%u\n", COUNT.addr, v.c_str(), CUR_ADDR.get_qty());   ///FUTURE: -qty
}

void        __prn_bk(void)  // TODO: hash
{
#ifndef DBG
    //time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    cout
        << "Bk: " << COUNT.bk
        << ", time: " << CUR_BK.head_ptr->time
        << ", hash: " << hash2hex(CUR_BK.hash)
        << ", ver: " << CUR_BK.head_ptr->ver
        << ", txs: " << CUR_BK.txs
        << endl;
        // << " (" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")"
#endif
}

void        __prn_tx(void)
{
#ifndef DBG
    cout
        << "  Tx: " << LOCAL.tx
        << " (" << COUNT.tx << ")"
        << ", hash: " << hash2hex(CUR_TX.hash)
        << ", ins: "  << CUR_TX.vins
        << ", outs: " << CUR_TX.vouts
        << ", ver: " << CUR_TX.ver
        << ", lock: " << CUR_TX.locktime
        << endl;
#endif
}

void        __prn_vin(void)
{
#ifndef DBG
    cout << "    Vin: " << LOCAL.vin
        << ", src: ";
    if (CUR_VIN.vout == 0xFFFFFFFF)
        cout << "<coinbase>";
    else
        cout << "(Tx: " << CUR_VIN.txno << ", vout: " << CUR_VIN.vout << ")";
    cout
        << ", ssize: " << CUR_VIN.ssize
        << ", seq: " << CUR_VIN.seq
        << endl;
#endif
}

void        __prn_vout(void)
{
#ifndef DBG
    cout
        << "    Vout: " << LOCAL.vout
        << ", no: " << LOCAL.vout
        << ", $: " << CUR_VOUT.satoshi
        << ", ssize: " << CUR_VOUT.ssize
        << endl;
#endif
}

void        __prn_addr(void)
{
    string v;
    auto alist = CUR_ADDR.get_strings();
    if (alist.size()) {
        sort(alist.begin(), alist.end());
        v = alist[0];
        for (size_t i = 1; i < alist.size(); i++)
            v = v + "," + alist[i];
    }
#ifndef DBG
    cout << "      Addr: " << CUR_ADDR.get_type_name();
    if (!v.empty())
        cout << " " << v;
    cout << endl;
#else
    printf("%u\t%u\t%u\t%s\t%s\n", COUNT.bk, LOCAL.tx, LOCAL.vout, CUR_ADDR.get_type_name(), v.c_str());
#endif
}

// ====
void    __prn_head(void)
{
  cerr << "Bk\tTx\tVins\tVouts\tAddrs\tUAddrs\tMem,M\tTime\n";
  __prn_tail();
}

void    __prn_tail(void)
{
  cerr << "---\t-------\t-------\t-------\t-------\t-------\t-------\t-----\n";
}

void    __prn_interim(void) {
    cerr <<
         (COUNT.bk+1)/1000 <<
         TAB << COUNT.tx <<
         TAB << STAT.vins <<
         TAB << STAT.vouts <<
         TAB << STAT.addrs <<
         TAB << COUNT.addr <<
         TAB << ((memused() - start_mem) >> 10) <<
         TAB << time(nullptr) - start_time <<
    "\n";
}

void    __prn_summary(void)
{
    cerr << "= Summary =" << endl
        << "Blocks:" << TAB << COUNT.bk << endl; //
    if (OPTS.verbose)   // >2
        cerr
            << "Tx:" << TAB << COUNT.tx << endl
            << "Vins:" << TAB << STAT.vins << endl
            << "Vouts:" << TAB << STAT.vouts << endl
            << "Addrs:" << TAB << STAT.addrs << endl
            << "Tx/bk max:" << TAB << STAT.max_txs << endl
            << "Vins/tx max:" << TAB << STAT.max_vins << endl
            << "Vouts/tx max:" << TAB << STAT.max_vouts << endl
            << "Addrs/vout max:" << TAB << STAT.max_addrs << endl;
}

bool    __prn_trace(void)
{
    cerr << "*** <Trace> ***" << endl;
    if (BUSY.bk)
        cerr << "Block:" << TAB << COUNT.bk << endl;
    if (BUSY.tx)
        cerr << "Tx:" << TAB << LOCAL.tx << "(" << COUNT.tx << ")" << endl;
    if (BUSY.vin)
        cerr << "Vin:" << TAB << CUR_VIN.txno << endl;
    if (BUSY.vout)
        cerr << "Vout:" << TAB << LOCAL.vout << endl;
    cerr << "*** </Trace> ***" << endl;
    return false;
}
