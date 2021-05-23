/**
 * unknown = nullptr
 */

#include <iostream>
#include <string>
#include <cstring>      // memcmp
#include <algorithm>    // sort
#include "bk/addr.h"
#include "bk/opcode.h"
//#include "bce.h"      // STAT, OPTS
#include "uintxxx.h"    // hash160

using namespace std;

const string ADDR_NULL_T::repr(void) {
  return string();
}

/// check PK (uncompressed) prefix
inline bool check_PKu_pfx(const u8_t pfx) {
  /*
   * Prefix byte must be 0x04, but there are some exceptions, e.g.
   * bk 230217
   * tx 657aecafe66d729d2e2f6f325fcc4acb8501d8f02512d1f5042a36dd1bbd21d1
   * vouts 34 (7), 100 (6), 158 (7), 360 (6)
   * but *not* 5 (vouts 189, 242)
   */
  return (pfx & 0xFC) == 0x04 and pfx != 0x05;
}

/// check PK (compressed) prefix (must be 2..3)
inline bool check_PKc_pfx(const u8_t pfx) {
  return (pfx & 0xFE) == 0x02;
}

ADDR_PK_T::ADDR_PK_T(string_view script) {
  auto opcode = script[0];
  if (opcode == 0x41) { // 0x42 == 65 == PKu
    if (script.length() == 67
        and check_PKu_pfx(script[1])
        and u8_t(script[66]) == OP_CHECKSIG)  // end signature
      hash160(script.data() + 1, 65, data);   // +=pfx_byte
    else
      throw AddrException("Bad P2PKu");
  } else {              // 0x21 == 33 == PKc
    if (script.length() == 35                   // compressed
        and check_PKc_pfx(script[1])
        and u8_t(script[34]) == OP_CHECKSIG)
      hash160(script.data() + 1, 33, data);   // +=pfx_byte
    else
      throw AddrException("Bad P2PKu");
  }
}

const string ADDR_PK_T::repr(void) {
  return ripe2addr(data);
}

ADDR_PKH_T::ADDR_PKH_T(string_view script) {
  if (script.length() == 25 and       // was >= dirty hack for 71036.?.? and w/ OP_NOP @ end
      u8_t(script[1]) == OP_HASH160 and
      u8_t(script[2]) == 20 and
      u8_t(script[23]) == OP_EQUALVERIFY and
      u8_t(script[24]) == OP_CHECKSIG)
    memcpy(&data, script.data() + 3, sizeof (data));
  else
    throw AddrException("Bad P2PKH");
}

const string ADDR_PKH_T::repr(void) {
  return ripe2addr(data);
}

ADDR_SH_T::ADDR_SH_T(string_view script) {
  if (script.length() == 23 and
      script[1] == 20 and
      u8_t(script[22]) == OP_EQUAL)
    memcpy(&data, script.data() + 2, sizeof (data));
  else
    throw AddrException("P2SH not implemented");
}

const string ADDR_SH_T::repr(void) {
  return ripe2addr(data, 5);
}

ADDR_WPKH_T::ADDR_WPKH_T(string_view script) {
  memcpy(&data, script.data() + 2, sizeof (data));
}

const string ADDR_WPKH_T::repr(void) {
  return wpkh2addr(data);
}

ADDR_WSH_T::ADDR_WSH_T(string_view script) {
  memcpy(&data, script.data() + 2, sizeof (data));
}

const string ADDR_WSH_T::repr(void) {
  return wsh2addr(data);
}

bool uint160_gt(const uint160_t &l, const uint160_t &r)    // desc
  { return memcmp(&l, &r, sizeof (uint160_t)) > 0; }

inline void sort_multisig(vector<uint160_t> v) {
  if (v.size() > 1)
    sort(v.begin(), v.end(), uint160_gt);
}

ADDR_MS_T::ADDR_MS_T(string_view script) {
  u8_t *script_ptr = (u8_t *) script.data();
  auto script_size = script.length();
  auto keys_qty_ptr = script_ptr + script_size - 2;
  auto keys_qty = *keys_qty_ptr - 0x50;
  auto key_ptr = script_ptr + 1;                        // key len
  if (u8_t(script_ptr[script_size-1]) == OP_CHKMULTISIG // 2nd signature
      and script_ptr[0] <= *keys_qty_ptr                // required (== opcode) <= qty
      and *keys_qty_ptr <= OP_16) {                     // max 16 keys
    for (auto i = 0; i < keys_qty and key_ptr < keys_qty_ptr; i++, key_ptr += (key_ptr[0]+1)) {
      if (
          ((*key_ptr == 0x41) and check_PKu_pfx(key_ptr[1])) or
          ((*key_ptr == 0x21) and check_PKc_pfx(key_ptr[1]))) {
        uint160_t tmp;
        hash160(key_ptr+1, key_ptr[0], tmp);
        data.push_back(tmp);
      } else
        throw AddrException("Bad P2MS: key #" + to_string(i));
    }
  } else
    throw AddrException("Bad P2MS");
  sort_multisig(data);
}

const string ADDR_MS_T::repr(void) {
  return string();
}

ADDR_BASE_T *addr_decode(string_view data) {  // sript, size
  // FIXME: empty script
  ADDR_BASE_T *retvalue = nullptr;
  u8_t opcode = data[0];
  if (opcode == OP_RETURN)
    return new ADDR_NULL_T();
  if (data.length() < 22)             // P2WPKH is smallest script
    return nullptr;
  switch (opcode) {
    case 0x41:
      retvalue = new ADDR_PK_T(data); // uncompressed
      break;
    case 0x21:
      retvalue = new ADDR_PK_T(data); // compressed
      break;
    case OP_DUP:
      retvalue = new ADDR_PKH_T(data);
      break;
    case OP_HASH160:
      retvalue = new ADDR_SH_T(data);
      break;
    case OP_0:                          // P2W* ver.0 (BIP-141)
      switch (data[1]) {
        case 0x14:
          retvalue = new ADDR_WPKH_T(data); // uint160_t
          break;
        case 0x20:
          retvalue = new ADDR_WSH_T(data);  // uint256_t
          break;
        default:
          throw AddrException("Bad P2Wx");
      }
      break;
    case OP_1 ... OP_16:
      retvalue = new ADDR_MS_T(data);
      break;
    default:
      if (opcode <= 0xB9) // x. last defined opcode
        ; // throw AddrException("Not impl-d");
      else
        throw AddrException("Invalid");
  }
  return retvalue;
}

