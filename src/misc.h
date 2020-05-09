#ifndef MISC_H
#define MISC_H

#include "bce.h"

int         cli(int, char *[]); ///< Handle CLI. Return 0 if error, argv's index of 1st filename on success.

uint32_t    read_v(void);
uint32_t    read_32(void);
uint64_t    read_64(void);
uint8_t    *read_u8_ptr(uint32_t);
uint32_t   *read_32_ptr(void);
uint256_t  *read_256_ptr(void);

string      hash2str(uint256_t &);
void        mk_hash(void *, uint32_t, uint256_t &); // 1.
//uint256_t   hash256(void *, uint32_t);            // 2.

void    out_vin(void);
void    out_vout(void);
void    out_tx(void);
void    out_bk(void);

void    __prn_vin(void);
void    __prn_vout(void);
void    __prn_tx(void);
void    __prn_bk(void);
void    __prn_file(string &);
void    __prn_summary(void);
string  ptr2hex(void *, size_t);
#endif // MISC_H
