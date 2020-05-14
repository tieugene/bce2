#include "kv.h"

bool        KVDB_T::init(string &s) {
    return db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE | kyotocabinet::PolyDB::OTRUNCATE); // TODO:
}

uint32_t    KVDB_T::size(void) {
    auto retvalue = db.count();
    if (retvalue < 0)
        return NOT_FOUND_U32;
    return uint32_t(retvalue);
}

uint32_t    TxDB_T::add(uint256_t &key) {      // FIXME: key only
    //auto value = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = size();
    if (value != NOT_FOUND_U32) {
        void *k_ptr = static_cast<void *>(&key);
        void *v_ptr = static_cast<void *>(&value);
        if (!db.add(static_cast<char *>(k_ptr), sizeof(uint256_t), static_cast<char *>(v_ptr), sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    }
    return value;
}

uint32_t    TxDB_T::get(uint256_t &key) {
    uint32_t value;
    void *k_ptr = static_cast<void *>(&key);
    void *v_ptr = static_cast<void *>(&value);
    auto result = db.get(static_cast<char *>(k_ptr), sizeof(uint256_t), static_cast<char *>(v_ptr), sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}

uint32_t    AddrDB_T::add(uint160_t &key) {
    //auto result = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    auto value = size();
    if (value != NOT_FOUND_U32) {
        void *k_ptr = static_cast<void *>(&key);
        void *v_ptr = static_cast<void *>(&value);
        if (!db.add(static_cast<char *>(k_ptr), sizeof(uint160_t), static_cast<char *>(v_ptr), sizeof(uint32_t)))
            value = NOT_FOUND_U32;
    }
    return value;
}

uint32_t    AddrDB_T::get(uint160_t &key) {
    uint32_t value;
    void *k_ptr = static_cast<void *>(&key);
    void *v_ptr = static_cast<void *>(&value);
    auto result = db.get(static_cast<char *>(k_ptr), sizeof(uint160_t), static_cast<char *>(v_ptr), sizeof(uint32_t));
    if (result != sizeof(uint32_t))
        value = NOT_FOUND_U32;
    return value;
}
