#include <time.h>
#include "bce.h"
#include "misc.h"
#include "printers.h"
#include "script.h"

void        out_bk(void)    ///< Output bk data for DB
{
    time_t t = static_cast<time_t>(CUR_BK.head_ptr->time);
    char dt[20];
    strftime(dt, 20, "%Y-%m-%d %OH:%OM:%OS", gmtime(&t));
    printf("b\t%u\t'%s'\t%s\n", COUNT.bk, dt, hash2hex(CUR_BK.hash).c_str());
    // cout << "b" << TAB << CUR_BK.no << TAB << "'" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << "'" << TAB << hash2hex(CUR_BK.hash) << endl;
}

void        out_tx(void)
{
  printf("t\t%u\t%u\t%s\n", COUNT.tx, COUNT.bk, hash2hex(CUR_TX.hash).c_str());
  // cout << "t" << TAB << CUR_TX.no << TAB << CUR_BK.no << TAB << hash2hex(CUR_TX.hash) << endl;
}

void        out_vin(void)   // FIXME: compare w/ COINBASE_txid too
{
    if (CUR_VIN.vout != COINBASE_vout)  // skip coinbase
      printf("i\t%u\t%llu\t%u\n", COUNT.tx, CUR_VIN.txno, CUR_VIN.vout);
      // cout << "i" << TAB << CUR_TX.no << TAB << CUR_VIN.txno << TAB << CUR_VIN.vout << endl;
      // hash2hex(*CUR_VIN.txid)
}

void        out_vout(void)
{
  printf("o\t%u\t%u\t%llu\n", COUNT.tx, LOCAL.vout, CUR_VOUT.satoshi);
  // cout << "o" << TAB << CUR_TX.no << TAB << CUR_VOUT.no << TAB << CUR_VOUT.satoshi << endl;
}

void        out_addr(uint32_t const id, uint160_t const &ripe)
{
  printf("a\t%u\t%s\n", id, ripe2addr(ripe).c_str());
  // cout << "a" << TAB << id << TAB << ripe2addr(ripe) << endl;
}

void        out_xaddr(uint32_t const id)
{
  printf("x\t%u\t%u\t%u\n", COUNT.tx, LOCAL.vout, id);
  // cout << "x" << TAB << CUR_TX.no << TAB << CUR_VOUT.no << TAB << id << endl;
}

void        __prn_vin(void)
{
    cerr << TAB << TAB << "Vin: ";
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
        << TAB << TAB << "Vout: "
        << "tx: " << COUNT.tx
        << ", no: " << LOCAL.vout
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

void        __prn_addr(void)    // FIXME:
{
    cerr
        << "Addr:"
        << "tx: " << COUNT.tx
        << ", vout: " << LOCAL.vout
        << ", ripe160: " << ripe2addr(CUR_ADDR.addr[0])
        << endl;
}

void        __prn_tx(void)
{
    cerr
        << "\tTx: " << COUNT.tx
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
        << "Block: " << COUNT.bk
        << ", time: " << CUR_BK.head_ptr->time
        << ", hash: " << hash2hex(CUR_BK.hash)
        << ", ver: " << CUR_BK.head_ptr->ver
        << ", txs: " << CUR_BK.txs
        << endl;
        // << " (" << put_time(gmtime(&t), "%Y-%m-%d %OH:%OM:%OS") << ")"
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
         TAB << (memused() - start_mem) / 1048576 <<
         TAB << time(nullptr) - start_time <<
    "\n";
}

void        __prn_summary(void)
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

bool        __prn_trace(void)
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
