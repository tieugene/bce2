/**
 * Order:
 * - Create items ("fast walk")
 * - Parse (w/ hashes (bk, tx); maybe multithread)
 * - Resolve from k-v [+ vvv]
 * - Save to k-v
 * - Print
 */
#ifndef BK_H
#define BK_H

#include <string_view>
#include <vector>
#include "common.h"

class ADDR_BASE_T {
public:
  ADDR_BASE_T();
};

class VIN_T {
private:
  const uint256_t  *tx_hash;
  uint32_t          vout;
  std::string_view  script;
  uint32_t          seq;
  uint64_t          tx_id;  // resolving
public:
  VIN_T(UNIPTR_T &);
  bool parse(void);
  bool resolve(void);
};

class VOUT_T {
private:
  uint64_t  satoshi;
  std::string_view script;
  ADDR_BASE_T *addr = nullptr;
public:
  VOUT_T(UNIPTR_T &);
  bool parse(void);
  bool resolve(void);
};

class WIT_T {
public:
  WIT_T(UNIPTR_T &);
};

class TX_T {
private:
  std::string_view data;  // for hash calc
  uint32_t ver;
  // bool segwit;
  std::vector<VIN_T> vins;
  std::vector<VOUT_T> vouts;
  std::vector<WIT_T> wits;
  void hash(void);
public:
  TX_T(UNIPTR_T &);
  bool parse(void);
  bool resolve(void);
};

class BK_T {
private:
  uint32_t height;
  std::string_view data;
  std::vector<TX_T> txs;
  void mk_hash(void);
public:
  BK_T(std::string_view, const uint32_t);
  bool parse(void);
  bool resolve(void);
};

#endif // BK_H
