// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BASE58_H
#define BASE58_H

//#include <stdint.h>
#include "common.h"

// from bitcoin-core 0.19.1/src/base58
// pbegin must be const...
/**
 * @brief EncodeBase58
 * @param pbegin
 * @param pend
 * @return
 */
std::string EncodeBase58(std::string_view data);

#endif // BASE58_H
