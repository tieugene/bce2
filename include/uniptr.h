#ifndef UNIPTR_H
#define UNIPTR_H

#include "common.h"

/// Universal ptr
union   UNIPTR_T {
    const void        *v_ptr;
    const char        *ch_ptr;
    const u8_t        *u8_ptr;
    const uint16_t    *u16_ptr;
    const uint32_t    *u32_ptr;
    const uint64_t    *u64_ptr;
    const uint160_t   *u160_ptr;
    const uint256_t   *u256_ptr;
    UNIPTR_T() {}
    UNIPTR_T(const char * ptr) { ch_ptr = ptr; }
    UNIPTR_T(const uint160_t * ptr) { u160_ptr = ptr; }
    UNIPTR_T(const uint256_t * ptr) { u256_ptr = ptr; }
    uint32_t          take_32(void) { return *u32_ptr++; }
    uint64_t          take_64(void) { return *u64_ptr++; }
    uint32_t          take_varuint(void);
    const u8_t        *take_u8_ptr(const uint32_t);
    const char        *take_ch_ptr(const uint32_t);
    const uint32_t    *take_32_ptr(void) { return u32_ptr++; }
    const uint256_t   *take_256_ptr(void) { return u256_ptr++; }
};

#endif // COMMON_H
