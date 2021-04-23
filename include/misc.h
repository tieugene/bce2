#ifndef MISC_H
#define MISC_H

#include "bce.h"
#include "uintxxx.h"

const string FROM_STDIN = "-";

bool        cli(int const, char *[]); ///< Handle CLI. Return 0 if error, argv's index of 1st filename on success.
long        memused(void);

uint32_t    read_v(void);
uint32_t    read_32(void);
uint64_t    read_64(void);
uint8_t     *read_u8_ptr(const uint32_t);
uint32_t    *read_32_ptr(void);
uint256_t   *read_256_ptr(void);

string      ptr2hex(const void *, const size_t);
string      str2hex(const string&);
int         hex2bytes(const string&, char *);

#endif // MISC_H
