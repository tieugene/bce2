#include <time.h>
#include "bce.h"
#include "printers.h"
#include "script.h"

void        out_vin(void)   // FIXME: compare w/ COINBASE_txid too
{
    if (CUR_VIN.vout != COINBASE_vout)  // skip coinbase
      printf("i\t%u\t%lu\t%u\n", CUR_TX.no, CUR_VIN.txno, CUR_VIN.vout);
      // cout << "i" << TAB << CUR_TX.no << TAB << CUR_VIN.txno << TAB << CUR_VIN.vout << endl;
      // hash2hex(*CUR_VIN.txid)
}

void        out_vout(void)
{
  printf("o\t%u\t%u\t%lu\n", CUR_TX.no, CUR_VOUT.no, CUR_VOUT.satoshi);
  // cout << "o" << TAB << CUR_TX.no << TAB << CUR_VOUT.no << TAB << CUR_VOUT.satoshi << endl;
}

void        out_addr(uint32_t id, uint160_t &ripe)
{
  printf("a\t%u\t%s\n", id, ripe2addr(ripe).c_str());
  // cout << "a" << TAB << id << TAB << ripe2addr(ripe) << endl;
}

void        out_xaddr(uint32_t id)
{
  printf("x\t%u\t%u\t%u\n", CUR_TX.no, CUR_VOUT.no, id);
  // cout << "x" << TAB << CUR_TX.no << TAB << CUR_VOUT.no << TAB << id << endl;
}

void        out_tx(void)
{
  printf("t\t%u\t%u\t%s\n", CUR_TX.no, CUR_BK.no, hash2hex(CUR_TX.hash).c_str());
  // cout << "t" << TAB << CUR_TX.no << TAB << CUR_BK.no << TAB << hash2hex(CUR_TX.hash) << endl;
}

void        out_bk(void)    ///< Output bk data for DB
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    char dt[20];
    strftime(dt, 20, "%Y-%m-%d %OH:%OM:%OS", gmtime(&t));
    printf("b\t%u\t'%s'\t%s\n", CUR_BK.no, dt, hash2hex(CUR_BK.hash).c_str());
    // cout << "b" << TAB << CUR_BK.no << TAB << "'" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << "'" << TAB << hash2hex(CUR_BK.hash) << endl;
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
    //time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
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
