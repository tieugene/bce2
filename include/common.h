#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <array>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <tuple>
#include <memory>

const uint32_t MAX_UINT32 = 0xFFFFFFFF;
typedef unsigned char u8_t;
typedef std::vector<u8_t> u8vector;
template <size_t Size>
using byte_array = std::array<u8_t, Size>;
typedef byte_array<32> uint256_t;   ///< hash representation
typedef byte_array<20> uint160_t;   ///< addr representation
typedef std::pair<std::unique_ptr<char[]>, uint32_t> uniq_view;
inline std::string_view u8string_view(const u8_t *s, size_t size) {
  return std::string_view((const char *) s, size);
}
inline std::string_view u256string_view(const uint256_t &key) {
  return u8string_view(std::data(key), sizeof(uint256_t));
}

class BCException : public std::runtime_error {
public:
  BCException(const std::string &msg) : std::runtime_error(msg) {}
  BCException(const char *msg) : std::runtime_error(msg) {}
};

/// Error shortcut (void)
void v_error(const std::string &);
/// Error shortcut (bool)
inline bool b_error(const std::string &s)
  { v_error(s); return false; }
/// Error shortcut (uint32_t)
inline u_int32_t u32_error(const std::string &s, uint32_t retcode = MAX_UINT32)
  { v_error(s); return retcode; }
inline std::string_view sv_error(const std::string &s)
  { v_error(s); return std::string_view(); }
inline uniq_view uv_error(const std::string &s)
  { v_error(s); return std::make_pair(nullptr, 0); }

#endif // COMMON_H
