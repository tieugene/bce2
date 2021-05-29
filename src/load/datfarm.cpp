#include <iomanip>
#include <iostream>
#include <sstream>
#include <queue>

#include "common.h"
#include "load/datfarm.h"

using namespace std;

/// Cached files number
const uint8_t WIN_SIZE = 16;
/// Cached files flags
static queue<size_t> opened;

bool  DATFARM_T::open(const size_t no) {
    if (no > qty)
      return b_error("File # too big: " + to_string(no));
    if (file[no].is_open())
      return true;
    if (opened.size() >= WIN_SIZE) {  // close unused file
      file[opened.front()].close();
      opened.pop();
    }
    ostringstream ss;
    ss << setw(5) << setfill('0') << no;
    auto fn = folder / (string("blk") + ss.str() + ".dat");
    file[no].open(fn, ios::in|ios::binary);
    if (!file[no].is_open())
      return b_error("Can't open file " + fn.string());
    opened.push(no);
    return true;
}

bool  DATFARM_T::read(const size_t no, const size_t offset, const int size, void *dst) {
    if (!open(no))
      return false;
    file[no].seekg(offset, file[no].beg);
    file[no].read(static_cast<char *>(dst), size);
    if (file[no].gcount() != size)
      return b_error("Can't read " + to_string(size) + " bytes from " + to_string(no) + "." + to_string(offset));
    return true;
}
