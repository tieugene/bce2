#ifndef KV_BASE_H
#define KV_BASE_H

#include <string_view>
#include "common.h"

const uint32_t NOT_FOUND_U32 = MAX_UINT32;

class KV_BASE_T {
public:
    virtual bool        init(const std::string &, uint64_t) = 0;
    virtual bool        close(void) = 0;
    virtual void        clear(void) = 0;
    virtual uint32_t    count(void) = 0;
    //bool        add(const string &key, const uint32_t value);
    /**
     * @brief Add new k-v pair
     * @param key Key to add
     * @return Value of new key added or NOT_FOUND_U32
     */
    virtual uint32_t    add(std::string_view key) = 0;
    virtual uint32_t    add(const uint256_t &key) = 0;  // tx, WSH
    /**
     * @brief Get value of key
     * @param key Key to find
     * @return Key value or NOT_FOUND_U32
     */
    virtual uint32_t    get(std::string_view key) = 0;
    virtual uint32_t    get(const uint256_t &key) = 0;
    /**
     * @brief Try to get existing k-v or add new
     * @param key Key to find
     * @return Found or added value or NOT_FOUND_U32 on error (not found nor added)
     * @throw Not found nor added, added as not expected
     */
    virtual uint32_t    get_or_add(std::string_view key) = 0;
};

#endif // KV_BASE_H
