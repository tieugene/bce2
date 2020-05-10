/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include "uintxxx.h"

extern uint160_t result;

/** Returns qty of decoded (-1 on error) */
int     script_decode(uint8_t *, uint32_t);

#endif // SCRIPT_H
