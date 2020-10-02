#ifndef MISC_H
#define MISC_H

#include "bce.h"
#include "uintxxx.h"

bool        cli(int const, char *[]); ///< Handle CLI. Return 0 if error, argv's index of 1st filename on success.
// inline bool job_mode(void) { return(!OPTS.cachedir.empty()); }
long        memused(void);

uint32_t    read_v(void);
uint32_t    read_32(void);
uint64_t    read_64(void);
uint8_t     *read_u8_ptr(const uint32_t);
uint32_t    *read_32_ptr(void);
uint256_t   *read_256_ptr(void);

string      ptr2hex(const void *, const size_t);
string      str2hex(const string&);

#endif // MISC_H
