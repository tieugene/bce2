#ifndef ADDR_H
#define ADDR_H

#include "bk/bk.h" // Addr_base_t

using namespace std;

ADDR_BASE_T *addr_decode(string_view);

class ADDR_NULL_T : public ADDR_BASE_T {
public:
  ADDR_NULL_T(void) {}
  const char *name(void) { return "nulldata"; }
};

class ADDR_PK_T : public ADDR_BASE_T {
public:
  ADDR_PK_T(std::string_view);
  const char *name(void) { return "pubkey"; }
};

class ADDR_PKH_T : public ADDR_BASE_T {
public:
  ADDR_PKH_T(std::string_view);
  const char *name(void) { return "pubkeyhash"; }
};

class ADDR_SH_T : public ADDR_BASE_T {
public:
  ADDR_SH_T(std::string_view);
  const char *name(void) { return "scripthash"; }
};

class ADDR_WPKH_T : public ADDR_BASE_T {
public:
  ADDR_WPKH_T(std::string_view);
  const char *name(void) { return "witness_v0_keyhash"; }
};

class ADDR_WSH_T : public ADDR_BASE_T {
public:
  ADDR_WSH_T(std::string_view);
  const char *name(void) { return "witness_v0_scripthash"; }
};

class ADDR_MS_T : public ADDR_BASE_T {
public:
  ADDR_MS_T(std::string_view);
  const char *name(void) { return "multisig"; }
};

#endif // ADDR_H
