/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include "uintxxx.h"

enum    SCTYPE {
    NULLDATA,
    PUBKEY,
    PUBKEYHASH,
    SCRIPTHASH,
    MULTISIG,       // []
    W0KEYHASH,
    W0SCRIPTHASH,   // WSH (0x20)
    NONSTANDARD
};

struct  ADDRS_T {
    SCTYPE      type;
    char        qty;
    uint160_t   addr[16];
};

extern ADDRS_T CUR_ADDR;
extern uint256_t WSH;           // hack: for P2WSH only

bool        script_decode(uint8_t *, const uint32_t);
const char *get_addrs_type(void);
std::vector<std::string> get_addrs_strs(void);

#endif // SCRIPT_H
