/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

#include <vector>
#include <array>
#include "uintxxx.h"

typedef std::vector<std::string> string_list;

enum    SCTYPE {    // script (address) type
    PUBKEYu,
    PUBKEYc,
    PUBKEYHASH,
    SCRIPTHASH,
    MULTISIG,       // PUBKEY[HASH][]
    W0KEYHASH,
    W0SCRIPTHASH,   // WSH (0x20)
    NULLDATA,
    NONSTANDARD
};

class ADDR_FOUND_T {
    // add addr: hash160 (P2PK, P2MS) / memcpy
    // get addr: for KC.add()/get()
private:
    uint32_t    id;
    SCTYPE      type;
    uint8_t     qty;
    uint16_t    len;
    union   {
        uint8_t     u8[16*sizeof(uint160_t)];    // max P2MS size
        std::array<uint160_t, 16>   u160;
        uint256_t   *u256;
    } buffer;
public:
    void                reset(void);
    inline void         set_id(const uint32_t v) { id = v; }
    inline uint32_t     get_id(void) { return id; }
    inline SCTYPE       get_type(void) { return type; }
    const char          *get_type_name(void);
    inline uint8_t      get_qty(void) { return qty; }
    const string_list   get_strings(void);
    void                add_data(const SCTYPE, const uint8_t *);
    inline uint16_t     get_len(void) { return len; }
    inline uint8_t      *get_data() { return buffer.u8; }
    void                sort_multisig(void);
};

bool        script_decode(uint8_t *, const uint32_t);

extern ADDR_FOUND_T CUR_ADDR;

#endif // SCRIPT_H
