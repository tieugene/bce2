#ifndef MISC_H
#define MISC_H

#include "bce.h"
#include "uintxxx.h"

bool        cli(int const, char *[]); ///< Handle CLI. Return 0 if error, argv's index of 1st filename on success.
inline bool job_mode(void) { return(!OPTS.cachedir.empty()); }

uint32_t    read_v(void);
uint32_t    read_32(void);
uint64_t    read_64(void);
uint8_t     *read_u8_ptr(uint32_t const);
uint32_t    *read_32_ptr(void);
uint256_t   *read_256_ptr(void);

string      ptr2hex(void const *, size_t const);

#endif // MISC_H
