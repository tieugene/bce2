// TODO: as_json, as_key, qty, is_full - inherited
#ifndef ADDR_H
#define ADDR_H

#include <string_view>
#include <memory>   // unique_ptr
#include "common.h"

typedef byte_array<21> short_key;   ///< addr key representation

class AddrException : public std::runtime_error {
public:
  AddrException(const std::string &msg) : std::runtime_error(msg) {}
  AddrException(const char *msg) : std::runtime_error(msg) {}
};

class ADDR_BASE_T {
public:
  //ADDR_BASE_T(string_view) = 0;
  virtual ~ADDR_BASE_T() {}
  virtual bool is_full(void) = 0;       // FIXME: class [const] attribute
  virtual u8_t qty(void) = 0;
  virtual const char *name(void) = 0;   // FIXME: class [const] attribute
  virtual const std::string_view as_key(void) = 0;
  virtual const std::string repr(void) = 0;
  virtual const std::string as_json(void) = 0;
};

class ADDR_NULL_T : public ADDR_BASE_T {
public:
  ADDR_NULL_T(void) {}
  bool is_full(void) { return false; }
  u8_t qty(void) { return 0; }
  const char *name(void) { return "nulldata"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key(void);
};

class ADDR_PK_T : public ADDR_BASE_T {
private:
  uint160_t data;
  short_key key;
public:
  ADDR_PK_T(std::string_view);
  bool is_full(void) { return true; }
  u8_t qty(void) { return 1; }
  const char *name(void) { return "pubkey"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key(void);
};

class ADDR_PKH_T : public ADDR_BASE_T {
private:
  uint160_t data;
  short_key key;
public:
  ADDR_PKH_T(std::string_view);
  bool is_full(void) { return true; }
  u8_t qty(void) { return 1; }
  const char *name(void) { return "pubkeyhash"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key(void);
};

class ADDR_SH_T : public ADDR_BASE_T {
private:
  uint160_t data;
  short_key key;
public:
  ADDR_SH_T(std::string_view);
  bool is_full(void) { return true; }
  u8_t qty(void) { return 1; }
  const char *name(void) { return "scripthash"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key(void);
};

class ADDR_WPKH_T : public ADDR_BASE_T {
private:
  uint160_t data;
  short_key key;
public:
  ADDR_WPKH_T(std::string_view);
  bool is_full(void) { return true; }
  u8_t qty(void) { return 1; }
  const char *name(void) { return "witness_v0_keyhash"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key(void);
};

class ADDR_WSH_T : public ADDR_BASE_T {
private:
  uint256_t data;
public:
  ADDR_WSH_T(std::string_view);
  bool is_full(void) { return true; }
  u8_t qty(void) { return 1; }
  const char *name(void) { return "witness_v0_scripthash"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key();
};

class ADDR_MS_T : public ADDR_BASE_T {
private:
  std::vector<uint160_t> data;
  short_key key1;
public:
  ADDR_MS_T(std::string_view);
  bool is_full(void) { return true; }
  u8_t qty(void) { return data.size(); }
  const char *name(void) { return "multisig"; }
  const std::string repr(void);
  const std::string as_json(void);
  const std::string_view as_key(void);
};

std::unique_ptr<ADDR_BASE_T> addr_decode(std::string_view);

#endif // ADDR_H
