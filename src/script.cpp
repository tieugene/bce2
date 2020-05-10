#include <iostream>
#include "script.h"

uint160_t   result;

static uint8_t  *cur;
static uint32_t last;

// opcodes
enum OPCODE : uint8_t {
    OP_0            = 0x00,
    OP_RETURN       = 0x6A,
    OP_DUP          = 0x76,
    OP_EQUAL        = 0x87,
    OP_EQUALVERIFY  = 0x88,
    OP_HASH160      = 0xA9,
    OP_CHECKSIG     = 0xAC
};

int     do_P2PK(void) {
    if (
        (last = 67) and
        (cur[66] = OP_CHECKSIG)
        )
        // result = cur+1; // FIXME: 2 x sha256 + ripeme-160
        return 1;
    return -1;
}

int     do_P2PKH(void) {
    if (
        (last = 25) and
        (cur[1] == OP_HASH160) and
        (cur[2] == 20) and
        (cur[23] == OP_EQUALVERIFY) and
        (cur[24] = OP_CHECKSIG)
        )
        // result = cur+3;
        return 1;
    return -1;
}

int     do_P2MS(void) {
    return -1;
}

int     do_P2SH(void) {
    if (
        (last = 23) and
        (cur[1] == 20) and
        (cur[22] == OP_EQUAL)
        )
        // result = cur+2;
        return 1;
    return -1;
}

int     do_P2ND(void) {
    return -1;
}

int     do_P2W(void) {
    return -1;
}

int     script_decode(uint8_t *script, uint32_t size)
{
    cur = script;
    last = size;
    switch (*cur) {
        case OP_DUP:            // 2. P2PKH
            return do_P2PKH();
            break;
        // case ???:
        // return do_P2MS();
        // break;
        case OP_HASH160:        // 4. P2SH
            return do_P2SH();
            break;
        case OP_RETURN:         // 5. NULL_DATA
            return 0;
            break;
        case 0:                 // x. witness*
            return -1;
            break;
        default:
            if (*cur <= 0x46)  // 1. P2PK
                return do_P2PK();
            cerr << "Unknown vout type: " << hex << *cur << endl;
            return -1;
    }
}
