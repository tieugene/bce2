#ifndef BKIDX_H
#define BKIDX_H

#include "common.h"

const uint32_t  MAX_BK_SIZE = 4 << 20;  // 4MB enough

size_t init_bkloader(const std::string, const std::string);
bool   load_bk(u8_t *, const uint32_t);
/**
 * @brief Read a block as hex string from stdin
 * @param dst Byted block destination
 * @param bkno Block number
 * @return true on success
 */
bool   stdin_bk(u8_t *dst, const uint32_t bkno);

#endif // BKIDX_H
