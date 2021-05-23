/**
 * unknown = nullptr
 */

#include <iostream>
#include <string>
#include "bk/addr.h"
#include "bk/opcode.h"
//#include "bce.h"      // STAT, OPTS
//#include "uintxxx.h"  // hash

using namespace std;

/// dummy
void dump_script(const string s) { cerr << s << endl; }

ADDR_BASE_T *addr_decode(string_view data) {  // sript, size
  /// FIXME: empty script
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
            dump_script("Bad P2Wx");
      }
      break;
  case OP_1 ... OP_16:
      retvalue = new ADDR_MS_T(data);
      break;
  default:
      if (opcode <= 0xB9) // x. last defined opcode
          dump_script("Not impl-d");
      else
          dump_script("Invalid");
  }
  return retvalue;
}

