#include "bce.h"
#include "handlers.h"
#include "misc.h"
#include "printers.h"
#include "script.h"

static uint32_t BK_GLITCH[] = {91722, 91842};    // dup 91880, 91812

bool    parse_tx(uint32_t bk_tx_no); // TODO: hash
bool    parse_vin(uint32_t no);
bool    parse_vout(uint32_t no);
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
    if (OPTS.out)
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
    if (OPTS.out)
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
    if (OPTS.out)
        out_vout();
    if (OPTS.verbose >= 4)
        __prn_vout();
    if (!parse_script())
        return false;
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
        if (OPTS.out)
            out_addr(addr_id, CUR_ADDR.addr);
        STAT.addrs += 1;
    }
    if (OPTS.out)
        out_xaddr(addr_id);
    if (OPTS.verbose >= 4)
        __prn_addr();
    return true;
}
