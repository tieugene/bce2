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

static const char * ScriptType_s[] = {  // for 0..550k
    "nonstandard",
    "nulldata",
    "pubkey",
    "pubkeyhash",
    "multisig",
    "scripthash",
    "witness_v0_scripthash",
    "witness_v0_keyhash"
};

ADDRS_T CUR_ADDR;
SCRIPT_TYPE_T ScriptType_n;

static uint8_t  *script_ptr;    // ptr to currently decoded opcode
static uint32_t script_size;    // script size

const char *get_cur_keytype(void)
{
    return ScriptType_s[ScriptType_n];
}

void    dump_script(const string s)
{
    cerr
        << "Script err: " << s << "\t("
        << "bk = " << COUNT.bk
        << ", tx = " << LOCAL.tx << " (" << COUNT.tx << ")"
        << ", vout = " << LOCAL.vout
        << ", script: " << ptr2hex(script_ptr, script_size)
        << ")" << endl;
}

bool    do_P2PK(uint8_t const opcode)   ///< ?pubkey
{
    if ((
            script_size == 67 and
            opcode == 0x41 and
            script_ptr[66] == OP_CHECKSIG
        ) or (
            script_size == 35 and
            opcode == 0x21 and
            script_ptr[34] == OP_CHECKSIG
        ))
    {
        hash160(script_ptr+1, script_size-2, CUR_ADDR.addr);
        CUR_ADDR.qty = 1;
        ScriptType_n = PUBKEY;
        return true;
    }
    dump_script("Bad P2PK");
    return false;
}

bool    do_P2PKH(void)                  ///< ?pubkeyhash
{
    //if (script_size == 5)      // very dirty hack for short 150951.*.* (PKH = 0x00)
    //    return true;
    if (
        script_size >= 25 and       // dirty hack for 71036.?.? and w/ OP_NOP @ end
        script_ptr[1] == OP_HASH160 and
        script_ptr[2] == 20 and
        script_ptr[23] == OP_EQUALVERIFY and
        script_ptr[24] == OP_CHECKSIG
        )
    {
        memcpy(&CUR_ADDR.addr, script_ptr+3, sizeof (uint160_t));
        CUR_ADDR.qty = 1;
        // if (ssize > 25)
        //    dump_script("P2PKH: Wrong script length");
        ScriptType_n = PUBKEYHASH;
        return true;
    }
    dump_script("Bad P2PKH");
    return false;
}

bool    do_P2SH(void) {                 ///< scripthash
    if (
        script_size == 23 and
        script_ptr[1] == 20 and
        script_ptr[22] == OP_EQUAL
        )
    {
        memcpy(&CUR_ADDR.addr, script_ptr+2, sizeof (uint160_t));
        CUR_ADDR.qty = 1;
        ScriptType_n = SCRIPTHASH;
        return true;
    }
    dump_script("Bad P2SH");
    return false;
}

bool    do_P2MS(void) {                 ///< multisig
    ScriptType_n = MULTISIG;
    dump_script("P2MS");
    return true;
}

bool    do_P2W(void) {
    dump_script("Witness");
    return true;
}

bool    script_decode(uint8_t * script, const uint32_t size)
{
    ScriptType_n = NONSTANDARD;
    if (size < 23)  // P2SH is smallest script
        return true;
    CUR_ADDR.qty = 0;
    script_ptr = script;
    script_size = size;
    auto opcode = *script_ptr;
    bool retvalue = false;
    switch (opcode) {
    case 0x01 ... 0x46:     // 1. P2PK
        retvalue = do_P2PK(opcode);
        retvalue = true;    /// forse ok
        break;
    case OP_DUP:            // 2. P2PKH
        retvalue = do_P2PKH();
        retvalue = true;    /// forse ok
        break;
    case OP_HASH160:        // 4. P2SH
        retvalue = do_P2SH();
        break;
    case OP_1:              // 3. P2MS
        retvalue = do_P2MS();
        break;
    case OP_RETURN:         // 5. NULL_DATA == nothing to do
        // FIXME:
        retvalue = true;
        break;
    case OP_0:              // x. witness*
        retvalue = do_P2W();
        break;
    default:
        if (opcode <= 0xB9) { // last defined opcode
            dump_script("Not impl-d");
            retvalue = true;   /// false
        } else {
            dump_script("Invalid");
            retvalue = true;
        }
    }
    return retvalue;
}
