/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include "uintxxx.h"

enum    SCTYPE {
    NONSTANDARD,
    NULLDATA,
    PUBKEY,
    PUBKEYHASH,
    MULTISIG,
    SCRIPTHASH,
    W0SCRIPTHASH,
    W0KEYHASH
};

struct  ADDRS_T {
    SCTYPE      type;
    char        qty;
    uint160_t   addr[16];
};

extern ADDRS_T CUR_ADDR;

bool        script_decode(uint8_t *, const uint32_t);
std::string get_addrs_str(void);
const char *get_addrs_type(void);

#endif // SCRIPT_H
