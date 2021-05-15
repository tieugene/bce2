/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include <vector>
#include <array>
#include <string_view>
#include "uintxxx.h"

/// Address[es] representation for printout (base58, bech32 etc)
typedef std::vector<std::string> string_list;
/// Longest address available (max P2MS size)
const uint16_t MAX_ADDR_LEN = 16*sizeof(uint160_t);

/// Script (address) type
enum    SCTYPE {
    PUBKEYu,        // P2PKu (65)
    PUBKEYc,        // P2PKc (33)
    PUBKEYHASH,     // P2PKH (20)
    SCRIPTHASH,     // P2SH (20)
    MULTISIG,       // P2MS, PUBKEYx[]
    W0KEYHASH,      // P2WPKH? (20)
    W0SCRIPTHASH,   // P2WSH (32)
    NULLDATA,
    NONSTANDARD
};

/// Container for address[es] decoded
class ADDR_FOUND_T {
    // add addr: hash160 (P2PK, P2MS) / memcpy
    // get addr: for KC.add()/get()
private:
    uint32_t    id;     ///< address id in K-V storage
    SCTYPE      type;   ///< address type
    uint8_t     qty;    ///< addresses quantity
    uint16_t    len;    ///< real address[es] size
    union   {
        u8_t      u8[MAX_ADDR_LEN];
        std::array<uint160_t, 16>   u160;
        uint256_t *u256;
    } buffer;           ///< address[es] found ready to K-V storage
public:
    void                reset(void);
    inline void         set_id(const uint32_t v) { id = v; }
    inline uint32_t     get_id(void) { return id; }
    inline SCTYPE       get_type(void) { return type; }
    const char         *get_type_name(void);
    inline uint8_t      get_qty(void) { return qty; }
    const string_list   get_strings(void);
    void                add_data(const SCTYPE, const u8_t *);
    inline uint16_t     get_len(void) { return len; }
    inline u8_t        *get_data() { return buffer.u8; }
    std::string_view    get_view(void) { return std::string_view((char *) buffer.u8, len); }
    void                sort_multisig(void);
};

/**
 * @brief Decode script into (ADDR_FOUND_T) CUR_ADDR
 * @param script Script to decode
 * @param size Size of script (bytes)
 * @return True on success
 * @todo string_view
 */
bool        script_decode(const u8_t *script, const uint32_t size);

extern ADDR_FOUND_T CUR_ADDR;

#endif // SCRIPT_H
