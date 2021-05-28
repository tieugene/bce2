#ifndef MISC_H
#define MISC_H

#include "bce.h"

/// Load options from config and CLI
bool        load_opts(int argc, char *[]);
/// Current resident memory usage (KB)
long        memused(void);
/// Convert bytes into hex string
std::string ptr2hex(std::string_view data);
/**
 * @brief Convert hash256 into hex string
 * @param src Data to hex
 * @return Hex string
 */
const std::string hash2hex(const uint256_t &src);                   // printers, handlers (err)
/**
 * @brief Convert hash160 into hex string
 * @param src Data to hex
 * @return Hex string
 */
const std::string ripe2hex(const uint160_t &src);                   // ---

#endif // MISC_H
