#include <stdio.h>  // printf

#include "bce.h"
#include "handlers.h"
#include "misc.h"
#include "printers.h"
#include "script.h"

static uint32_t BK_GLITCH[] = {91722, 91842};    // dup 91880, 91812

bool    parse_tx(void); // TODO: hash
bool    parse_vin(const bool);
bool    parse_vout(const bool);
bool    parse_script(void);

bool    parse_bk(void)
{
    BUSY.bk = true;
    CUR_BK.head_ptr = static_cast<BK_HEAD_T*> (CUR_PTR.v_ptr);
    CUR_PTR.u8_ptr += sizeof (BK_HEAD_T);
    CUR_BK.txs = read_v();
    if (OPTS.out) // on demand
    {
        hash256(CUR_BK.head_ptr, sizeof(BK_HEAD_T), CUR_BK.hash);
        if (OPTS.cash)
            out_bk();
        else
            __prn_bk();
    }
    if (!(COUNT.bk == BK_GLITCH[0] or COUNT.bk == BK_GLITCH[1]))
        for (LOCAL.tx = 0; LOCAL.tx < CUR_BK.txs; LOCAL.tx++, COUNT.tx++)
            if (!parse_tx())
                return false;
    STAT.max_txs = max(STAT.max_txs, CUR_BK.txs);
    BUSY.bk = false;
    return true;
}

bool    parse_tx(void) // TODO: hash
{
    BUSY.tx = true;
    auto h_beg = CUR_PTR.u8_ptr;
    CUR_TX.ver = read_32();
    // 1. fast rewind
    CUR_TX.vins = read_v();
    auto tmp_ptr = CUR_PTR.v_ptr;
    for (uint32_t i = 0; i < CUR_TX.vins; i++)
        parse_vin(false);
    CUR_TX.vouts = read_v();
    for (uint32_t i = 0; i < CUR_TX.vouts; i++)
        parse_vout(false);
    CUR_TX.locktime = read_32();
    // real parse
    if (OPTS.out or OPTS.cash)
    {
        auto h_end = CUR_PTR.u8_ptr;
        hash256(h_beg, h_end - h_beg, CUR_TX.hash);
        if (OPTS.cash) {
            auto tx_added = TxDB.add(CUR_TX.hash);
            if (tx_added == NOT_FOUND_U32) {
                    cerr << "Can't add tx " << hash2hex(CUR_TX.hash) << endl;
                    return false;
            }
            if (tx_added != COUNT.tx) {
                    cerr << "Tx " << hash2hex(CUR_TX.hash) << " added as " << tx_added << " against waiting " << COUNT.tx << endl;
                    return false;
            }
            if (OPTS.out)
                out_tx();
        } else  // cashless & out
            __prn_tx();
    }
    // real parse
    CUR_PTR.v_ptr = tmp_ptr;    // after vins
    for (LOCAL.vin = 0; LOCAL.vin < CUR_TX.vins; LOCAL.vin++)
        if (!parse_vin(true))
            return false;
    read_v();   // vouts
    for (LOCAL.vout = 0; LOCAL.vout < CUR_TX.vouts; LOCAL.vout++)
        if (!parse_vout(true))
            return false;
    read_32();  // locktime
    STAT.vins += CUR_TX.vins;
    STAT.vouts += CUR_TX.vouts;
    STAT.max_vins = max(STAT.max_vins, CUR_TX.vins);
    STAT.max_vouts = max(STAT.max_vouts, CUR_TX.vouts);
    BUSY.tx = false;
    return true;
}

bool    parse_vin(const bool dojob)
{
    // FIXME: coinbase = 32x00 + 4xFF (txid+vout)
    CUR_VIN.txid = read_256_ptr();
    CUR_VIN.vout = read_32();
    CUR_VIN.ssize = read_v();
    CUR_VIN.script = read_u8_ptr(CUR_VIN.ssize);
    CUR_VIN.seq = read_32();
    if (!dojob)
        return true;
    BUSY.vin = true;
    if (OPTS.cash)
    {
        if (CUR_VIN.vout != COINBASE_vout) {
            CUR_VIN.txno = TxDB.get(*CUR_VIN.txid);
            if (CUR_VIN.txno == NOT_FOUND_U32) {
                cerr << "txid " << hash2hex(*CUR_VIN.txid) << " not found." << endl;
                return false;
            }
        }
        if (OPTS.out)
            out_vin();
    } else if (OPTS.out)
        __prn_vin();
    BUSY.vin = false;
    return true;
}

bool    parse_vout(const bool dojob)
{
    CUR_VOUT.satoshi = read_64();
    CUR_VOUT.ssize = read_v();
    CUR_VOUT.script = read_u8_ptr(CUR_VOUT.ssize);
    if (!dojob)
        return true;
    BUSY.vout = true;
    if (!parse_script())
        return false;
    if (OPTS.out)
    {
        if (OPTS.cash)
            out_vout();
        else
            __prn_vout();
    }
    BUSY.vout = false;
    return true;
}

bool    parse_script(void)
{
    if (!script_decode(CUR_VOUT.script, CUR_VOUT.ssize))
        return false;    // !!! TERMPORARY !!!
        //printf("%d\t%d\t%d\tbad\t%s\n", COUNT.bk, LOCAL.tx, LOCAL.vout, get_cur_keytype());
    // <get_addrs>
    printf("%d\t%d\t%d\t%s", COUNT.bk, LOCAL.tx, LOCAL.vout, get_cur_keytype());
    if (CUR_ADDR.qty) {
        printf("\t%s", ripe2addr(CUR_ADDR.addr[0], (CUR_ADDR.type == SCRIPTHASH) ? 5 : 0).c_str());
        for (auto i = 1; i < CUR_ADDR.qty; i++)
            printf(",%s", ripe2addr(CUR_ADDR.addr[i]).c_str());
    };
    printf("\n");
    // </get_addrs>
    // FIXME: cashless
    if (OPTS.cash)
    {
        auto addr_added = AddrDB.get(CUR_ADDR.addr[0]);
        if (addr_added == NOT_FOUND_U32) {
            addr_added = AddrDB.add(CUR_ADDR.addr[0]);
            if (addr_added == NOT_FOUND_U32) {
                cerr << "Can not find nor add addr " << ripe2hex(CUR_ADDR.addr[0]) << endl;
                return false;
            }
            if (addr_added != COUNT.addr) {
                    cerr << "Addr added as " << addr_added << " against waiting " << COUNT.addr << endl;
                    return false;
            }
            if (OPTS.out)
                out_addr(addr_added, CUR_ADDR.addr[0]);
            COUNT.addr += 1;
        }
    } else if (OPTS.out)
        __prn_addr();
    // if (OPTS.out)
    //    out_xaddr(addr_added);
    STAT.addrs += 1;
    return true;
}
