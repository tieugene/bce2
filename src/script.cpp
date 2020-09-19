/* Script decoder
 * RTFM:
 * - https://en.bitcoin.it/wiki/Script
 * - https://learnmeabitcoin.com/guide/script
 * Excludes (bk tx vout):
 * - 71036
 * - 140921 (x3)
 * - 141460.13.0
 * - 150951.1.0 (empty P2PKH)
 * - 154012.25.2 - dirty PKH
 */
#include <iostream>
#include <cstring>
#include "bce.h"
#include "misc.h"
#include "script.h"
#include "opcode.h"

ADDRS_T CUR_ADDR;

static uint8_t  *script_ptr;   // ptr to currently decoded opcode
static uint32_t script_size;  // script size

void    dump_script(const string s)
{
    cerr
        << "Script err: " << s << " ("
        << "bk = " << CUR_BK.no
        << ", tx = " << CUR_TX.bkno << " (" << CUR_TX.no << ")"
        << ", vout = " << CUR_VOUT.no
        << ", script: " << ptr2hex(script_ptr, script_size)
        << ")" << endl;
}

bool    do_P2PK(uint8_t const opcode) {
    if (
        script_size == 67 and
        opcode == 0x41 and
        script_ptr[66] == OP_CHECKSIG
        ) {
        hash160(script_ptr+1, 65, CUR_ADDR.addr);
        CUR_ADDR.qty = 1;
        return true;
    }
    if (CUR_BK.no == 140921)    // dirty hack (skip "nonstandart")
        return true;
    dump_script("Wrong P2PK");
    return false;
}

bool    do_P2PKH(void) {
    if (script_size == 5)      // very dirty hack for 150951.*.* (PKH = 0x00)
        return true;
    if (
        script_size >= 25 and // dirty hack for 71036.?.? and w/ OP_NOP @ end
        script_ptr[1] == OP_HASH160 and
        script_ptr[2] == 20 and
        script_ptr[23] == OP_EQUALVERIFY and
        script_ptr[24] == OP_CHECKSIG
        ) {
        memcpy(&CUR_ADDR.addr, script_ptr+3, sizeof (uint160_t));
        CUR_ADDR.qty = 1;
        // if (ssize > 25)
        //    dump_script("P2PKH: Wrong script length");
        return true;
    }
    dump_script("Wrong P2PKH");
    return false;
}

bool    do_P2MS(void) {
    dump_script("P2MS detected");
    return true;
}

bool    do_P2SH(void) {
    if (
        script_size == 23 and
        script_ptr[1] == 20 and
        script_ptr[22] == OP_EQUAL
        ) {
        memcpy(&CUR_ADDR.addr, script_ptr+2, sizeof (uint160_t));
        CUR_ADDR.qty = 1;
        return true;
    }
    dump_script("Wrong P2SH");
    return false;
}

bool    do_P2W(void) {
    dump_script("Witness");
    return true;
}

bool    script_decode(uint8_t * const script, uint32_t const size)
{
    CUR_ADDR.qty = 0;
    script_ptr = script;
    script_size = size;
    auto opcode = *script_ptr;
    bool retvalue = false;
    switch (opcode) {
    case 0x01 ... 0x46:     // 1. P2PK (obsolet)
        retvalue = do_P2PK(opcode);
        break;
    case OP_DUP:            // 2. P2PKH
        retvalue = do_P2PKH();
        break;
    case OP_1:              // 3. P2MS
        retvalue = do_P2MS();
        break;
    case OP_HASH160:        // 4. P2SH
        retvalue = do_P2SH();
        break;
    case OP_RETURN:         // 5. NULL_DATA == nothing to do
        retvalue = true;
        break;
    case OP_0:              // x. witness*
        retvalue = do_P2W();
        break;
    default:
        if (opcode <= 0xB9) { // last defined opcode
            if (CUR_BK.no == 141460) {    // dirty hack (tx.13, skip "nonstandart")
                retvalue = true;
            }  else {
                dump_script("Not implemented");
                retvalue = false;
            }
        } else {
            dump_script("Script invalid");
            retvalue = true;
        }
    }
    return retvalue;
}
