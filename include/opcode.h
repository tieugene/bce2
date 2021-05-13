#ifndef OPCODE_H
#define OPCODE_H

#include "common.h"

enum OPCODE : u8_t {
    OP_0            = 0x00,
    OP_1            = 0x51,
    OP_2            = 0x52,
    OP_3            = 0x53,
    OP_4            = 0x54,
    OP_5            = 0x55,
    OP_6            = 0x56,
    OP_7            = 0x57,
    OP_8            = 0x58,
    OP_9            = 0x59,
    OP_10           = 0x5A,
    OP_11           = 0x5B,
    OP_12           = 0x5C,
    OP_13           = 0x5D,
    OP_14           = 0x5E,
    OP_15           = 0x5F,
    OP_16           = 0x60,
    OP_NOP          = 0x61,
    OP_RETURN       = 0x6A,
    OP_DUP          = 0x76,
    OP_EQUAL        = 0x87,
    OP_EQUALVERIFY  = 0x88,
    OP_HASH160      = 0xA9,
    OP_CHECKSIG     = 0xAC,
    OP_CHKMULTISIG  = 0xAE,
    OP_NOP10        = 0xB9  // last defined opcode
};

#endif // OPCODE_H
