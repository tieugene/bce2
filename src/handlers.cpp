#include "bce.h"
#include "handlers.h"
#include "misc.h"
#include "printers.h"
#include "script.h"

static uint32_t BK_GLITCH[] = {91722, 91842};    // dup 91880, 91812

bool    parse_tx(const uint32_t bk_tx_no); // TODO: hash
bool    parse_vin(const uint32_t no);
bool    parse_vout(const uint32_t no);
bool    parse_script(void);

bool    parse_bk(void)
{
    CUR_BK.head_ptr = static_cast<BK_HEAD_T*> (CUR_PTR.v_ptr);
    CUR_PTR.u8_ptr += sizeof (BK_HEAD_T);
    CUR_BK.txs = read_v();
    if (OPTS.out or OPTS.verbose >= 2) // on demand
        hash256(CUR_BK.head_ptr, sizeof(BK_HEAD_T), CUR_BK.hash);
    if (OPTS.out)
        out_bk();
    if (OPTS.verbose >= 2)
        __prn_bk();
    if (!(COUNT.bk == BK_GLITCH[0] or COUNT.bk == BK_GLITCH[1]))
        for (LOCAL.tx = 0; LOCAL.tx < CUR_BK.txs; LOCAL.tx++, COUNT.tx++)
            if (!parse_tx(LOCAL.tx))
                return false;
    STAT.max_txs = max(STAT.max_txs, CUR_BK.txs);
    return true;
}

bool    parse_tx(const uint32_t bk_tx_no) // TODO: hash
{
    BUSY.tx = true;
    auto h_beg = CUR_PTR.u8_ptr;
    // CUR_TX.bkno = bk_tx_no;
    CUR_TX.ver = read_32();
    CUR_TX.vins = read_v();
    for (LOCAL.vin =  0; LOCAL.vin < CUR_TX.vins; LOCAL.vin++)
        if (!parse_vin(LOCAL.vin))
            return false;
    CUR_TX.vouts = read_v();
    for (LOCAL.vout = 0; LOCAL.vout < CUR_TX.vouts; LOCAL.vout++)
        if (!parse_vout(LOCAL.vout))
            return false;
    CUR_TX.locktime = read_32();
    auto h_end = CUR_PTR.u8_ptr;
    hash256(h_beg, h_end - h_beg, CUR_TX.hash);
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
    if (OPTS.verbose >= 3)
        __prn_tx();
    STAT.vins += CUR_TX.vins;
    STAT.vouts += CUR_TX.vouts;
    STAT.max_vins = max(STAT.max_vins, CUR_TX.vins);
    STAT.max_vouts = max(STAT.max_vouts, CUR_TX.vouts);
    BUSY.tx = false;
    return true;
}

bool    parse_vin(const uint32_t no)
{
    // FIXME: coinbase = 32x00 + 4xFF (txid+vout)
    BUSY.vin = true;
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
    if (OPTS.out)
        out_vin();
    if (OPTS.verbose >= 4)
        __prn_vin();
    BUSY.vin = false;
    return true;
}

bool    parse_vout(const uint32_t no)
{
    // TODO: out_addr
    BUSY.vout = true;
    CUR_VOUT.satoshi = read_64();
    CUR_VOUT.ssize = read_v();
    CUR_VOUT.script = read_u8_ptr(CUR_VOUT.ssize);
    if (OPTS.out)
        out_vout();
    if (OPTS.verbose >= 4)
        __prn_vout();
    if (!parse_script())
        return false;
    BUSY.vout = false;
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
        if (addr_id != COUNT.addr) {
                cerr << "Addr added as " << addr_id << " against waiting " << COUNT.addr << endl;
                return false;
        }
        if (OPTS.out)
            out_addr(addr_id, CUR_ADDR.addr);
        COUNT.addr += 1;
    }
    STAT.addrs += 1;
    if (OPTS.out)
        out_xaddr(addr_id);
    if (OPTS.verbose >= 4)
        __prn_addr();
    return true;
}
