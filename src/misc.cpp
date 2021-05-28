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
#include "bk/script.h" // cur_addr only
#if defined(__APPLE__)
#include <mach/mach.h>
#endif

using namespace std;

/// Returns used memory in kilobytes
long get_statm(void) {
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

long memused(void) {
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

const string  hash2hex(const uint256_t &h) {
  UNIPTR_T u(&h);
  char tmp[65];
  tmp[64] = '\0';
  sprintf(tmp,
#if defined(__APPLE__)
    "%016llx%016llx%016llx%016llx",
#else
    "%016lx%016lx%016lx%016lx",
#endif
    u.u64_ptr[3], u.u64_ptr[2], u.u64_ptr[1], u.u64_ptr[0]);
  return string(tmp);
}

const string  ripe2hex(const uint160_t &r) {
  UNIPTR_T u(&r);
  char tmp[41];
  tmp[40] = '\0';
  sprintf(tmp, "%08x%08x%08x%08x%08x", u.u32_ptr[4], u.u32_ptr[3], u.u32_ptr[2], u.u32_ptr[1], u.u32_ptr[0]);
  return string(tmp);
}
