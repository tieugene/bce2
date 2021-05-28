#ifndef BKIDX_H
#define BKIDX_H

#include <filesystem>
#include "common.h"

/// Max block size, bytes
const uint32_t  MAX_BK_SIZE = 4 << 20;  // 4MB enough
/**
 * @brief Init blocks direct loader
 * @param datdir Path to *.dat files folder
 * @param locsfile Path to locs-file
 * @return Blocks number locations loaded about
 */
size_t init_bkloader(const std::filesystem::path datdir, const std::filesystem::path locsfile);
/**
 * @brief Load block directly
 * @param dst Buffer load to
 * @param bk_no Block height
 * @return True on success
 */
std::string_view load_bk(const uint32_t bk_no);
/**
 * @brief Read a block as hex string from stdin
 * @param dst Byted block destination
 * @param bk_no Block height
 * @return True on success
 */
std::string_view stdin_bk(const uint32_t bk_no);

#endif // BKIDX_H
