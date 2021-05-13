/*
 * Misc utilities
 * TODO: options:
 * - input bk no=>hash table
 */

//#include <cstdlib>
//#include <cstring>
//#include <stdio.h>
#include <unistd.h>
//#include <filesystem>
#include "bce.h"
#include "misc.h"
#include "script.h" // cur_addr only
#include "fasthex.h"
#if defined(__APPLE__)
#include <mach/mach.h>
#endif

using namespace std;

long        get_statm(void) {   ///< returns used memory in kilobytes
    long    total = 0;  // rss, shared, text, lib, data, dt; man proc
#if defined (__linux__)
    ifstream statm("/proc/self/statm");
    statm >> total; // >> rss...
    statm.close();
    total *= (sysconf(_SC_PAGE_SIZE) >> 10);  // pages-ze = 4k in F32_x64
#elif defined(__APPLE__)
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    if (KERN_SUCCESS == task_info(mach_task_self(),
        TASK_BASIC_INFO, (task_info_t)&t_info,
        &t_info_count))
        total = t_info.virtual_size >> 10;
#endif
    return total;
}

long        memused(void) {
    return get_statm();
}

string  ptr2hex(string_view data) {
    static string hex_chars = "0123456789abcdef";
    string s;
    auto cptr = data.cbegin();
    for (size_t i = 0; i < data.length(); i++, cptr++) {
        s.push_back(hex_chars[(*cptr & 0xF0) >> 4]);
        s.push_back(hex_chars[(*cptr & 0x0F)]);
    }
    return s;
}

int hex2bytes(string_view s, u8_t *const dst) {
  auto src_ptr = s.cbegin();
  auto src_end = src_ptr + s.length();
  auto *dst_ptr = dst;

  for (; src_ptr < src_end; src_ptr += 2, dst_ptr++)
    *dst_ptr = (hextoint(src_ptr[0]) << 4) | hextoint(src_ptr[1]);
  return dst_ptr - dst;
}
