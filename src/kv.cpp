#include "kv.h"

//unordered_map<uint256_t, uint64_t>  map;

/*uint32_t    TxMap_T::size(void) {
    return map.size();
}*/

bool        TxMap_T::add(uint256_t &key, uint32_t value) {
    //auto result = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    return true;
}

uint32_t    TxMap_T::get(uint256_t &key) {
    uint32_t retvalue = NOT_FOUND_U32;
    /*
    auto got = map.find(key);
    if (got != map.end())
        retvalue = got->second;
    */
    return retvalue;
}

// ==== KyotoCabinet
bool        TxDB_T::init(string &s) {
    return db.open(s, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE);
}

uint64_t    TxDB_T::size(void) {
    auto retvalue = db.count();
    if (retvalue <= 0)
        return 0;
    return uint64_t(retvalue);
}

bool        TxDB_T::add(uint256_t &key, uint32_t value) {
    //auto result = map.emplace(key, value);   // FIXME: emplace() w/ checking retvalue
    void *k_ptr = static_cast<void *>(&key);
    void *v_ptr = static_cast<void *>(&value);
    return db.add(static_cast<char *>(k_ptr), sizeof(uint256_t), static_cast<char *>(v_ptr), sizeof(uint32_t));
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
