/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include "uintxxx.h"

struct  ADDRS_T {
    char        qty;
    uint160_t   addr;
};

enum    SCRIPT_TYPE_T {
    NONSTANDARD,
    PUBKEY,
    PUBKEYHASH,
    MULTISIG,
    SCRIPTHASH
};

extern ADDRS_T CUR_ADDR;
extern SCRIPT_TYPE_T ScriptType_n;

bool    script_decode(uint8_t *, const uint32_t);
const char *get_cur_keytype(void);

#endif // SCRIPT_H
