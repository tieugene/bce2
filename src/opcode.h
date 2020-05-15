#ifndef OPCODE_H
#define OPCODE_H

enum OPCODE : uint8_t {
    OP_0            = 0x00,
    OP_1            = 0x51,
    OP_2            = 0x52,
    OP_3            = 0x53,
    OP_4            = 0x54,
    OP_5            = 0x55,
    OP_6            = 0x56,
    OP_7            = 0x57,
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
