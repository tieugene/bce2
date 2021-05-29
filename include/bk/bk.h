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
#include "addr.h"

class TX_T;
class BK_T {
private:
  uint32_t height;
  uint32_t time = 0;
  uint256_t hash = {0};
  std::string_view data;
  std::vector<TX_T*> txs;
  void mk_hash(void);
  friend void out_bk(const BK_T &);
  friend void prn_bk(const BK_T &);
public:
  // TODO: delete data in destructor
  BK_T(std::string_view, const uint32_t);
  ~BK_T();
  bool parse(void);
  bool resolve(void);
  inline uint32_t get_id(void) { return height; };
};

class VIN_T;
class VOUT_T;
class WIT_T;
class TX_T {
private:
  BK_T * const bk = nullptr;
  uint32_t no;
  uint32_t id = MAX_UINT32;
  std::string_view data;  // for hash calc
  uint256_t hash = {0};
  uint32_t ver = 0;
  bool segwit = false;
  uint32_t wit_offset = 0; ///< cut off for hash calc
  std::vector<VIN_T*> vins;
  std::vector<VOUT_T*> vouts;
  std::vector<std::unique_ptr<WIT_T>> wits;
  void mk_hash(void);
  friend void out_tx(const TX_T &);
  friend void prn_tx(const TX_T &);
public:
  TX_T(UNIPTR_T &, const uint32_t, BK_T * const);
  ~TX_T();
  bool parse(void);
  bool resolve(void);
  inline uint32_t get_no(void) { return no; };
  inline uint32_t get_id(void) { return id; };
  inline BK_T * get_bk(void) { return bk; }
};

class VIN_T {
private:
  TX_T * const tx = nullptr;
  uint32_t no;
  const uint256_t  *tx_hash = nullptr;
  uint32_t          vout = 0;
  std::string_view  script;
  uint32_t          seq = 0;
  uint32_t          tx_id = MAX_UINT32;  // resolving
  friend void out_vin(const VIN_T &);
  friend void prn_vin(const VIN_T &);
public:
  VIN_T(UNIPTR_T &, const uint32_t, TX_T * const);
  bool parse(void);
  bool resolve(void);
};

class VOUT_T {
private:
  TX_T * const tx = nullptr;
  uint32_t no;
  uint32_t addr_id = MAX_UINT32;  // aka NOT_FOUND
  uint64_t  satoshi = 0;
  std::string_view script;
  std::unique_ptr<ADDR_BASE_T> addr = nullptr;
  bool addr_is_new = false;   // for out_addr
  const std::string addr_type(void);
  friend void out_vout(const VOUT_T &);
  friend void prn_vout(VOUT_T &);
  friend void out_addr(const VOUT_T &);
public:
  VOUT_T(UNIPTR_T &, const uint32_t, TX_T * const);
  bool parse(void);
  bool resolve(void);
  //const std::string addr_repr(void);
};

class WIT_T {
private:
  TX_T * const tx = nullptr;
  uint32_t no;
public:
  WIT_T(UNIPTR_T &, const uint32_t, TX_T * const);
};

#endif // BK_H
