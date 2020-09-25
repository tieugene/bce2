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
commit:
- return strict pk[h] and sh check
- pk: chk prefix
- todo: ms: chk prefixes
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

static uint8_t  *script_ptr;    // ptr to currently decoded opcode
static uint32_t script_size;    // script size

const char *get_addrs_type(void)
{
    return ScriptType_s[CUR_ADDR.type];
}

string  get_addrs_str(void)
{
    string retvalue;
    if (CUR_ADDR.qty) {
        retvalue = ripe2addr(CUR_ADDR.addr[0], (CUR_ADDR.type == SCRIPTHASH) ? 5 : 0);
        if (CUR_ADDR.type == MULTISIG)
            for (auto i = 1; i < CUR_ADDR.qty; i++)
                retvalue = retvalue + "," + ripe2addr(CUR_ADDR.addr[i]);
    }
    return retvalue;
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

// FIXME: https://learnmeabitcoin.com/technical/public-key
// 65x (uncompressed) - stars with 0x4
// 33x (compressed) - starts with 0x2/0x3
bool    do_P2PK(void)                   ///< pubkey
{
    if ((
            script_size == 67
            and script_ptr[0] == 0x41           // uncompressed
            and script_ptr[1] == 0x04           // prefix
            and script_ptr[66] == OP_CHECKSIG   // end signature
        ) or (
            script_size == 35                   // compressed
            and script_ptr[0] == 0x21
            and (script_ptr[1] & 0xFE) == 0x02  // prefix = 2..3
            and script_ptr[34] == OP_CHECKSIG
        ))
    {
        CUR_ADDR.type = PUBKEY;
        CUR_ADDR.qty = 1;
        hash160(script_ptr+1, script_size-2, CUR_ADDR.addr[0]);
        return true;
    }
    dump_script("Bad P2PK");
    return false;
}

bool    do_P2PKH(void)                  ///< ?pubkeyhash
{
    if (
        script_size == 25 and       // was >= dirty hack for 71036.?.? and w/ OP_NOP @ end
        script_ptr[1] == OP_HASH160 and
        script_ptr[2] == 20 and
        script_ptr[23] == OP_EQUALVERIFY and
        script_ptr[24] == OP_CHECKSIG
        )
    {
        CUR_ADDR.type = PUBKEYHASH;
        CUR_ADDR.qty = 1;
        memcpy(&CUR_ADDR.addr[0], script_ptr+3, sizeof (uint160_t));
        return true;
    }
    dump_script("Bad P2PKH");
    return false;
}

bool    do_P2SH(void)                   ///< scripthash
{
    if (
        script_size == 23 and
        script_ptr[1] == 20 and
        script_ptr[22] == OP_EQUAL
        )
    {
        CUR_ADDR.type = SCRIPTHASH;
        CUR_ADDR.qty = 1;
        memcpy(&CUR_ADDR.addr[0], script_ptr+2, sizeof (uint160_t));
        return true;
    }
    dump_script("Bad P2SH");
    return false;
}

bool    do_P2MS(void)                   ///< multisig
{
    auto keys_qty_ptr = script_ptr + script_size - 2;
    auto keys_qty = *keys_qty_ptr - 0x50;
    auto retvalue = false;
    auto key_ptr = script_ptr + 1;  // key len
    //cout << msize << ": " << script_size << "==" << (5 + msize * 65) << endl;   //2:135=135,3:201=200, 16:1059=1045
    if (
        script_ptr[script_size-1] == OP_CHKMULTISIG     // 2nd signature
        and script_ptr[0] <= *keys_qty_ptr                  // required (== opcode) <= qty
        and *keys_qty_ptr <= OP_16                          // max 16 keys
//        and script_size == (3 + msize_num * 66)
       )
    {
        for (auto i = 0; i < keys_qty and *key_ptr == 0x41 and key_ptr < keys_qty_ptr; i++, key_ptr += (key_ptr[0]+1))
            hash160(key_ptr+1, *key_ptr, CUR_ADDR.addr[i]);
    }
    if (key_ptr == keys_qty_ptr) {
        CUR_ADDR.type = MULTISIG;
        CUR_ADDR.qty = keys_qty;
        retvalue = true;
    } else
        dump_script("Bad P2MS");
    return retvalue;
}

bool    do_P2W(void)
{
    dump_script("Witness");
    return true;
}

bool    script_decode(uint8_t *script, const uint32_t size)
{
    CUR_ADDR.type = NONSTANDARD;
    CUR_ADDR.qty = 0;
    if (size < 23)  // P2SH is smallest script
        return true;
    CUR_ADDR.qty = 0;
    script_ptr = script;
    script_size = size;
    auto opcode = *script_ptr;
    bool retvalue = false;
    switch (opcode) {
    case 0x01 ... 0x46:     // 1. P2PK
        retvalue = do_P2PK();
        retvalue = true;    /// forse ok
        break;
    case OP_DUP:            // 2. P2PKH 0x76
        retvalue = do_P2PKH();
        retvalue = true;    /// forse ok
        break;
    case OP_HASH160:        // 3. P2SH 0xA9
        retvalue = do_P2SH();
        retvalue = true;    /// forse ok
        break;
    case OP_1 ... OP_16:    // 4. P2MS 0x5x
        retvalue = do_P2MS();
        retvalue = true;
        break;
    case OP_RETURN:         // 5. NULL_DATA
        CUR_ADDR.type = NULLDATA;
        retvalue = true;
        break;
    case OP_0:              // 6. x. witness* 0x00
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
