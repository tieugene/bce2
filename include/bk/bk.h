/**
 * Order:
 * - Create items ("fast walk")
 * - Parse (w/ hashes; maybe multithread)
 * - Resolve from k-v [+ vvv]
 * - Save to k-v
 * - Print
 */
#ifndef BK_H
#define BK_H

#include <vector>

class ADDR_BASE_T {
public:
  ADDR_BASE_T();
};

class VOUT_T {
private:
  ADDR_BASE_T *addr = nullptr;
public:
  uint64_t satoshi;
  VOUT_T();
  /// Resolve addr to k-v
  void resolve(void);
};

class VIN_T {
public:
  VIN_T();
  /// Resolve tx from k-v
  void resolve(void);
};

class TX_T {
private:
  std::vector<VIN_T> vin;
private:
  std::vector<VOUT_T> vout;
public:
  TX_T();
};

class BK_T {
private:
  std::vector<TX_T> tx;
public:
  BK_T();
};

#endif // BK_H
