#ifndef ADDR_H
#define ADDR_H

#include <string_view>
#include <stdexcept>
#include "common.h"

class AddrException : public std::runtime_error {
public:
  AddrException(const std::string &msg) : std::runtime_error(msg) {}
  AddrException(const char *msg) : std::runtime_error(msg) {}
};

class ADDR_BASE_T {
public:
  //ADDR_BASE_T(string_view) = 0;
  virtual ~ADDR_BASE_T() {}
  virtual const char *name(void) = 0;
  virtual const std::string repr(void) = 0;
  //virtual const std::string_view &key() = 0;
};

class ADDR_NULL_T : public ADDR_BASE_T {
public:
  ADDR_NULL_T(void) {}
  const char *name(void) { return "nulldata"; }
  const std::string repr(void);
  //const std::string_view &key();
};

class ADDR_PK_T : public ADDR_BASE_T {
private:
  uint160_t data;
public:
  ADDR_PK_T(std::string_view);
  const char *name(void) { return "pubkey"; }
  const std::string repr(void);
  //const std::string_view &key();
};

class ADDR_PKH_T : public ADDR_BASE_T {
private:
  uint160_t data;
public:
  ADDR_PKH_T(std::string_view);
  const char *name(void) { return "pubkeyhash"; }
  const std::string repr(void);
  //const std::string_view &key();
};

class ADDR_SH_T : public ADDR_BASE_T {
private:
  uint160_t data;
public:
  ADDR_SH_T(std::string_view);
  const char *name(void) { return "scripthash"; }
  const std::string repr(void);
  //const std::string_view &key();
};

class ADDR_WPKH_T : public ADDR_BASE_T {
private:
  uint160_t data;
public:
  ADDR_WPKH_T(std::string_view);
  const char *name(void) { return "witness_v0_keyhash"; }
  const std::string repr(void);
  //const std::string_view &key();
};

class ADDR_WSH_T : public ADDR_BASE_T {
private:
  uint256_t data;
public:
  ADDR_WSH_T(std::string_view);
  const char *name(void) { return "witness_v0_scripthash"; }
  const std::string repr(void);
  //const std::string_view &key();
};

class ADDR_MS_T : public ADDR_BASE_T {
public:
  ADDR_MS_T(std::string_view);
  const char *name(void) { return "multisig"; }
  const std::string repr(void);
  //const std::string_view &key();
};

ADDR_BASE_T *addr_decode(std::string_view);

#endif // ADDR_H
