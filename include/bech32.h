// Copyright (c) 2017 Pieter Wuille
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Bech32 is a string encoding format used in newer address types.
// The output consists of a human-readable part (alphanumeric), a
// separator character (1), and a base32 data section, the last
// 6 characters of which are a checksum.
//
// For more information, see BIP 173.

#ifndef BECH32_H
#define BECH32_H

#include <stdint.h>
#include <string>
#include "common.h"

/** Encode a Bech32 string. If hrp contains uppercase characters, this will cause an assertion error. */
std::string Bech32Encode(const std::vector<u8_t> &);

#endif // BECH32_H
