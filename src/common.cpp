#include <iostream>
#include "common.h"

const u8_t *UNIPTR_T::take_u8_ptr(uint32_t const size) {
    auto retvalue = u8_ptr;
    u8_ptr += size;
    return retvalue;
}

///<read 1..4-byte int and forward;
uint32_t    UNIPTR_T::take_varuint(void) {
    auto retvalue = static_cast<uint32_t>(*u8_ptr++);
    if ((retvalue & 0xFC) == 0xFC) {
        switch (retvalue & 0x03) {
            case 0: // 0xFC
                break;
            case 1: // 0xFD
                retvalue = static_cast<uint32_t>(*u16_ptr++);
                break;
            case 2: // 0xFE
                retvalue = *u32_ptr++;
                break;
            case 3: // 0xFF
                throw BCException("Value 0xFF too big");
        }
    }
    return retvalue;
}

void v_error(const std::string &s) {
  std::cerr << "Err: " << s << std::endl;
}
