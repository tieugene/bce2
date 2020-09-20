/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include "uintxxx.h"

struct  ADDRS_T {
    char        qty;
    uint160_t   addr;
};

extern ADDRS_T CUR_ADDR;

bool    script_decode(uint8_t *, const uint32_t);

#endif // SCRIPT_H
