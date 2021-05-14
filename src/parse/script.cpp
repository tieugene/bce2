/* Script decoder
 * RTFM:
 * - https://en.bitcoin.it/wiki/Script
 * - https://learnmeabitcoin.com/guide/script
 * - bitcoin-core-x/src/key-io.cpp
 */
#include <iostream>
#include <cstring>  // memset
#include <algorithm>
//include <unistr.h> // u8_cmp
#include "bce.h"
#include "misc.h"
#include "script.h"
#include "opcode.h"
#include "encode.h"

using namespace std;

enum    KYE_TYPE_T {
    KEY_0,
    KEY_S,
    KEY_W
};

void    dump_script(const string &);

ADDR_FOUND_T CUR_ADDR;

static const u8_t  *script_ptr;    // ptr to currently decoded opcode
static uint32_t script_size;    // script size

bool cmp_uint160(const uint160_t &l, const uint160_t &r)    // desc
    { return memcmp(&l, &r, sizeof (uint160_t)) > 0; }

const char *ADDR_FOUND_T::get_type_name(void)
{
    static const char *ScriptType_s[] = {  // for 0..550k
        "pubkey",
        "pubkey",
        "pubkeyhash",
        "scripthash",
        "multisig",
        "witness_v0_keyhash",
        "witness_v0_scripthash",
        "nulldata",
        "nonstandard"
    };
    return ScriptType_s[CUR_ADDR.type];
}

void ADDR_FOUND_T::reset(void)
{
    type = NONSTANDARD;
    qty = 0;
    len = 0;
    memset(buffer.u8, 0, sizeof (buffer.u8));
}

const string_list ADDR_FOUND_T::get_strings(void)
{
    string_list retvalue;
    switch (type) {
    case PUBKEYu:
    case PUBKEYc:
    case PUBKEYHASH:
        retvalue.push_back(ripe2addr(buffer.u8 + 1));
        break;
    case SCRIPTHASH:
        retvalue.push_back(ripe2addr(buffer.u8 + 1, 5));
        break;
    case W0KEYHASH:
        retvalue.push_back(wpkh2addr(buffer.u8 + 1));
        break;
    case W0SCRIPTHASH:
        retvalue.push_back(wsh2addr(buffer.u8));
        break;
    case MULTISIG:
        for (uint8_t i = 0; i < CUR_ADDR.qty; i++)
            retvalue.push_back(ripe2addr(buffer.u8 + i * sizeof(uint160_t)));
        break;
    default:    // nulldata, nonstandard
        ;
    }
    return retvalue;
}

void ADDR_FOUND_T::add_data(const SCTYPE t, const u8_t *src)
{
    type = t;
    switch (t) {
    case NONSTANDARD:
    case NULLDATA:
        qty = len = 0;
        break;
    case PUBKEYu:
        qty = 1;
        len = sizeof (uint160_t) + 1;
        buffer.u8[0] = KEY_0;
        hash160(src, 65, buffer.u8 + 1);
        break;
    case PUBKEYc:
        qty = 1;
        len = sizeof (uint160_t) + 1;
        buffer.u8[0] = KEY_0;
        hash160(src, 33, buffer.u8 + 1);
        break;
    case PUBKEYHASH:
        qty = 1;
        len = sizeof (uint160_t) + 1;
        buffer.u8[0] = KEY_0;
        memcpy(buffer.u8 + 1, src, sizeof (uint160_t));
        break;
    case SCRIPTHASH:
        qty = 1;
        len = sizeof (uint160_t) + 1;
        buffer.u8[0] = KEY_S;  // !!! 1 => add 0x05 on base58
        memcpy(buffer.u8 + 1, src, sizeof (uint160_t));
        break;
    case W0KEYHASH:
        qty = 1;
        len = sizeof (uint160_t) + 1;
        buffer.u8[0] = KEY_W;
        memcpy(buffer.u8 + 1, src, sizeof (uint160_t));
        break;
    case W0SCRIPTHASH:
        qty = 1;
        len = sizeof (uint256_t);
        memcpy(buffer.u8, src, sizeof (uint256_t));
        break;
    case MULTISIG:  // starting from key_len (!)
        hash160(src + 1, src[0], buffer.u8 + qty * sizeof(uint160_t));
        qty++;
        len += sizeof (uint160_t);
        break;
    }
}

inline void ADDR_FOUND_T::sort_multisig(void)
{
    if (qty > 1)
        std::sort(buffer.u160.begin(), buffer.u160.end(), cmp_uint160);
}

inline bool chk_PKu_pfx(const u8_t pfx)    // check PKu prefix
{
    /*
     * Prefix byte must be 0x04, but there are some exceptions, e.g.
     * bk 230217
     * tx 657aecafe66d729d2e2f6f325fcc4acb8501d8f02512d1f5042a36dd1bbd21d1
     * vouts 34 (7), 100 (6), 158 (7), 360 (6)
     * but *not* 5 (vouts 189, 242)
     */
    return (pfx & 0xFC) == 0x04 and pfx != 0x05;
}

inline bool chk_PKc_pfx(const u8_t pfx)    // check PKu prefix
{
    // prefix byte == 2..3
    return (pfx & 0xFE) == 0x02;
}

bool    do_P2PKu(void)                  ///< pubkey (uncompressed)
{
    // https://learnmeabitcoin.com/technical/public-key
    if (script_size == 67
        and chk_PKu_pfx(script_ptr[1])
        and script_ptr[66] == OP_CHECKSIG)   // end signature
    {
        CUR_ADDR.add_data(PUBKEYu, script_ptr+1);   // &pfx_byte
        return true;
    }
    dump_script("Bad P2PKu");
    return false;
}

bool    do_P2PKc(void)                  ///< pubkey (compressed)
{
    if (script_size == 35                   // compressed
        and chk_PKc_pfx(script_ptr[1])
        and script_ptr[34] == OP_CHECKSIG)
    {
        CUR_ADDR.add_data(PUBKEYc, script_ptr+1);
        return true;
    }
    dump_script("Bad P2PKc");
    return false;
}

bool    do_P2PKH(void)                  ///< pubkeyhash
{
    if (script_size == 25 and       // was >= dirty hack for 71036.?.? and w/ OP_NOP @ end
        script_ptr[1] == OP_HASH160 and
        script_ptr[2] == 20 and
        script_ptr[23] == OP_EQUALVERIFY and
        script_ptr[24] == OP_CHECKSIG)
    {
        CUR_ADDR.add_data(PUBKEYHASH, script_ptr+3);
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
        CUR_ADDR.add_data(SCRIPTHASH, script_ptr+2);
        return true;
    }
    dump_script("Bad P2SH");
    return false;
}

bool    do_P2WPKH(void)                 ///< witness_v0_keyhash
{
    CUR_ADDR.add_data(W0KEYHASH, script_ptr+2);
    return true;
}

bool    do_P2WSH(void)                  ///< witness_v0_scripthash
{
    CUR_ADDR.add_data(W0SCRIPTHASH, script_ptr+2);
    return true;
}

bool    do_P2MS(void)                   ///< multisig
{
    auto keys_qty_ptr = script_ptr + script_size - 2;
    auto keys_qty = *keys_qty_ptr - 0x50;
    auto retvalue = false;
    auto key_ptr = script_ptr + 1;  // key len
    //cout << msize << ": " << script_size << "==" << (5 + msize * 65) << endl;   //2:135=135,3:201=200, 16:1059=1045
    if (script_ptr[script_size-1] == OP_CHKMULTISIG     // 2nd signature
        and script_ptr[0] <= *keys_qty_ptr              // required (== opcode) <= qty
        and *keys_qty_ptr <= OP_16)                     // max 16 keys
    {
        for (auto i = 0; i < keys_qty and key_ptr < keys_qty_ptr; i++, key_ptr += (key_ptr[0]+1)) {
            if (((*key_ptr == 0x41) and chk_PKu_pfx(key_ptr[1]))
             or ((*key_ptr == 0x21) and chk_PKc_pfx(key_ptr[1]))) {
                if (keys_qty == 1)    // special case: 1-in-1
                    CUR_ADDR.add_data((*key_ptr == 0x41) ? PUBKEYu : PUBKEYc, key_ptr+1);   // starting from byte_pfx
                else
                    CUR_ADDR.add_data(MULTISIG, key_ptr);   // starting from key_len
             } else
                break;
        }
    }
    if (key_ptr == keys_qty_ptr) {    // final chk
        CUR_ADDR.sort_multisig();
        retvalue = true;
    } else {
        CUR_ADDR.reset();
        dump_script("Bad P2MS");
    }
    return retvalue;
}

bool    script_decode(const u8_t *script, const uint32_t size)
{
    /// FIXME: empty script
    CUR_ADDR.reset();
    auto opcode = *script;
    if (opcode == OP_RETURN) {  // 0. NULLDATA
        CUR_ADDR.add_data(NULLDATA, nullptr);
        return true;
    }
    if (size < 22)          // P2WPKH is smallest script
        return true;
    script_ptr = script;
    script_size = size;
    bool retvalue = false;
    switch (opcode) {
    case 0x41:              // 1. P2PK (uncompressed)
        retvalue = do_P2PKu();
        break;
    case 0x21:              // 2. P2PK (compressed)
        retvalue = do_P2PKc();
        break;
    case OP_DUP:            // 3. P2PKH 0x76
        retvalue = do_P2PKH();
        break;
    case OP_HASH160:        // 4. P2SH 0xA9
        retvalue = do_P2SH();
        break;
    case OP_0:              // 5. P2W* ver.0 (BIP-141)
        switch (script_ptr[1]) {
        case 0x14:          // 5.1. P2WPKH (uint160_t)
            retvalue = do_P2WPKH();
            break;
        case 0x20:          // 5.2. P2WSH (uint256_t)
            retvalue = do_P2WSH();
            break;
        default:
            dump_script("Bad P2Wx");
            retvalue = false;
        }
        break;
    case OP_1 ... OP_16:    // 4. P2MS 0x5x
        retvalue = do_P2MS();
        break;
    default:
        if (opcode <= 0xB9) // x. last defined opcode
            dump_script("Not impl-d");
        else
            dump_script("Invalid");
    }
    return retvalue;
}

void    dump_script(const string &s)
{
    if (OPTS.verbose == DBG_MAX)
        cerr
            << "Script err: " << s << "\t("
            << "bk = " << COUNT.bk
            << ", tx = " << LOCAL.tx << " (" << COUNT.tx << ")"
            << ", vout = " << LOCAL.vout
            << ", script: " << ptr2hex(u8string_view(script_ptr, script_size))
            << ")" << endl;
}
