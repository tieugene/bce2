/*
 * Block body processors
 */
#include <stdio.h>  // printf
#include "script.h"

#include "bce.h"
#include "handlers.h"
#include "misc.h"
#include "printers.h"
#include <cstring>

using namespace std;

static uint32_t BK_GLITCH[] = {91722, 91842};    // dup 91880, 91812

bool    parse_tx(void); // TODO: hash
bool    parse_vin(const bool);
bool    parse_vout(const bool);
bool    parse_wit();
bool    parse_script(void);

bool    parse_bk(void)
{
    BUSY.bk = true;
    CUR_BK.head_ptr = static_cast<const BK_HEAD_T*> ((const void *) CUR_PTR.take_u8_ptr(sizeof (BK_HEAD_T)));
    CUR_BK.txs = CUR_PTR.take_varuint();
    if (OPTS.out) // on demand
    {
        hash256(CUR_BK.head_ptr, sizeof(BK_HEAD_T), CUR_BK.hash);
        if (kv_mode())
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

bool    hash_tx(const uint8_t *tx_beg)   // calc tx hash on demand
{
    // CUR_PTR.v_ptr == &CUR_TX.vin[0]
    auto backup_ptr = CUR_PTR.v_ptr;    // tmp ptr to go back after hash calc
    // 1. fast rewind
    for (uint32_t i = 0; i < CUR_TX.vins; i++)
        parse_vin(false);
    CUR_TX.vouts = CUR_PTR.take_varuint();
    if (!CUR_TX.vouts)
        return b_error("Vouts == 0");
    for (uint32_t i = 0; i < CUR_TX.vouts; i++)
        parse_vout(false);
    if (!CUR_TX.segwit) {
        CUR_TX.locktime = CUR_PTR.take_32();
        hash256(tx_beg, CUR_PTR.u8_ptr - tx_beg, CUR_TX.hash);
    } else {
        auto wit_ptr = CUR_PTR.u8_ptr;
        for (uint32_t i = 0; i < CUR_TX.vins; i++)
            parse_wit();
        auto lt_ptr = CUR_PTR.u8_ptr;
        CUR_TX.locktime = CUR_PTR.take_32();
        // prepare buffer for hashing
        auto tmp_size = wit_ptr - tx_beg - sizeof(uint16_t) + sizeof(uint32_t);     // -segwit_sign +locktime
        uint8_t tmp_buf[tmp_size]; // , *tmp_ptr = tmp_buf;
        memcpy(tmp_buf, tx_beg, sizeof(uint32_t));                                  // tx.ver
        auto vin_ptr = tx_beg + sizeof(uint32_t) + sizeof(uint16_t);                // +ver +segwit_sign
        memcpy(tmp_buf + sizeof(uint32_t), vin_ptr, wit_ptr - vin_ptr);             // vins & vouts
        memcpy(tmp_buf + tmp_size - sizeof (uint32_t), lt_ptr, sizeof (uint32_t));  // locktime
        // hash it
        hash256(tmp_buf, tmp_size, CUR_TX.hash);
    }
    CUR_PTR.v_ptr = backup_ptr;    // after vins
    return true;
}

bool    parse_tx(void) // TODO: hash
{
    BUSY.tx = true;
    auto tx_beg = CUR_PTR.u8_ptr;
    CUR_TX.ver = CUR_PTR.take_32();
    CUR_TX.segwit = (*CUR_PTR.u16_ptr == 0x0100);
    if (CUR_TX.segwit)
        CUR_PTR.u16_ptr++;  // skip witness signature
    CUR_TX.vins = CUR_PTR.take_varuint();
    if (CUR_TX.vins == 0)
        return b_error("Vins == 0");
    if (OPTS.out or kv_mode()) {
        if (!hash_tx(tx_beg))
            return false;
        if (kv_mode()) {
          auto tx_added = TxDB->add(CUR_TX.hash);
          if (tx_added == NOT_FOUND_U32)
            return b_error("Can't add tx " + hash2hex(CUR_TX.hash));
          if (tx_added != COUNT.tx)
            return b_error("Tx " + hash2hex(CUR_TX.hash) + " added as " + to_string(tx_added) + " against waiting " + to_string(COUNT.tx));
          if (OPTS.out)
            out_tx();
        } else  // cashless & out
          __prn_tx();
    }
    for (LOCAL.vin = 0; LOCAL.vin < CUR_TX.vins; LOCAL.vin++)
        if (!parse_vin(true))
            return false;
    CUR_TX.vouts = CUR_PTR.take_varuint();   // vouts
    for (LOCAL.vout = 0; LOCAL.vout < CUR_TX.vouts; LOCAL.vout++)
        if (!parse_vout(true))
            return false;
    if (CUR_TX.segwit)
        for (LOCAL.wit = 0; LOCAL.wit < CUR_TX.vins; LOCAL.wit++)
            parse_wit();
    CUR_TX.locktime = CUR_PTR.take_32();  // locktime
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
    CUR_VIN.txid = CUR_PTR.take_256_ptr();
    CUR_VIN.vout = CUR_PTR.take_32();
    CUR_VIN.ssize = CUR_PTR.take_varuint();
    CUR_VIN.script = CUR_PTR.take_u8_ptr(CUR_VIN.ssize);
    CUR_VIN.seq = CUR_PTR.take_32();
    if (!dojob)
        return true;
    BUSY.vin = true;
    if (kv_mode())
    {
        if (CUR_VIN.vout != COINBASE_vout) {
            CUR_VIN.txno = TxDB->get(*CUR_VIN.txid);
            if (CUR_VIN.txno == NOT_FOUND_U32)
                return b_error("txid " + hash2hex(*CUR_VIN.txid) + " not found.");
        }
        if (OPTS.out)
            out_vin();
    } else if (OPTS.out)
        __prn_vin();
    BUSY.vin = false;
    return true;
}

bool    parse_wit()
{
    auto count = CUR_PTR.take_varuint();
    for (uint32_t i = 0; i < count; i++)
        CUR_PTR.u8_ptr += CUR_PTR.take_varuint();
    return true;
}

bool    parse_vout(const bool dojob)
{
    CUR_VOUT.satoshi = CUR_PTR.take_64();
    CUR_VOUT.ssize = CUR_PTR.take_varuint();
    CUR_VOUT.script = CUR_PTR.take_u8_ptr(CUR_VOUT.ssize);
    if (!dojob)
        return true;
    BUSY.vout = true;
    if (!parse_script())
        return false;
    if (OPTS.out) {
        if (kv_mode())
            out_vout();
        else
            __prn_vout();
    }
    BUSY.vout = false;
    return true;
}

bool    parse_script(void)
{
    /// FIXME: nulldata is not spendable
    /// FIXME: empty script
    auto script_ok = script_decode(CUR_VOUT.script, CUR_VOUT.ssize);
    if (script_ok and CUR_ADDR.get_qty()) {
        if (kv_mode()) {
            auto addr_tried = AddrDB->get_or_add(CUR_ADDR.get_view());
            // TODO: check created > expected
            if (addr_tried == NOT_FOUND_U32)
              return false;
            CUR_ADDR.set_id(addr_tried);
            if (addr_tried == COUNT.addr) {   // new addr created
              if (OPTS.out)
                out_addr();
              COUNT.addr++;
            }
        } else if (OPTS.out)
          __prn_addr();
        STAT.addrs += 1;    // FIXME: if decoded and 1+
    }
    return true;
}
