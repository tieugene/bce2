#ifndef MISC_H
#define MISC_H

#include "bce.h"

uint32_t    read_v(void);
uint32_t    read_32(void);
uint64_t    read_64(void);
uint8_t *   read_u8_ptr(uint32_t);
uint256_t * read_256_ptr(void);

void    mk_hash(void *, uint32_t, uint256_t &);
string  hash2str(uint256_t &);

void    out_vin(void);
void    out_vout(void);
void    out_tx(void);
void    out_bk(void);

void    __prn_vin(void);
void    __prn_vout(void);
void    __prn_tx(void);
void    __prn_bk(void);

#endif // MISC_H
