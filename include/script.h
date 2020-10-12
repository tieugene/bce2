/// Decoding vout's scripts

#ifndef SCRIPT_H
#define SCRIPT_H

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
    uint8_t     buffer[sizeof (uint160_t) * 16];    // max P2MS size
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
    inline uint8_t      *get_data() { return buffer; }
};

bool        script_decode(uint8_t *, const uint32_t);

extern ADDR_FOUND_T CUR_ADDR;

/*
struct  ADDRS_T {
    uint32_t    id;
    SCTYPE      type;
    char        qty;
    uint160_t   addr[16];
};

const char *get_addrs_type(void);
std::vector<std::string> get_addrs_strs(void);

extern ADDRS_T    CUR_ADDR;
extern uint256_t  WSH;           // hack: for P2WSH only
*/

#endif // SCRIPT_H
