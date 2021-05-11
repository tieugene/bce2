/*
 * locs- and *.dat files handler
 */
#include <iostream>
//#include <format>
#include <filesystem>
//#include <cstdint>
//#include <fstream>

#include "bkidx.h"
#include "datfarm.h"
#include "misc.h"

using namespace std;

FOFF_T    *FOFF;
DATFARM_T *datfarm;
const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE

///< load file-offset file
size_t  load_fileoffsets(const string fn) {
    ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (!file)            // 1. open
      return u32_error("File '" + fn + "' opening failed", 0);
    auto data_size = file.tellg();
    if ((data_size < 0) or (data_size & 0x7) or (data_size > (8 << 20)))    // 2. chk filesize
      return u32_error("Wrong file size (<0 or != 8x or >8MB (1M bks)): " + fn + "=" + to_string(data_size), 0);
    auto blocks = size_t(data_size >> 3);
    FOFF = new FOFF_T[blocks];
    if (!FOFF)
      return u32_error("Can't allocate mem for file-offset list.", 0);
    file.seekg (0, ios::beg);
    auto tmp = reinterpret_cast<char *>(FOFF);
    file.read (tmp, data_size);
    file.close();
    return blocks;
}

size_t  init_bkloader(const std::string datdir, const std::string locsfile) {
  auto bk_qty = load_fileoffsets(locsfile);
  if (bk_qty)
    datfarm = new DATFARM_T(bk_qty, (datdir.back() == '/') ? datdir : datdir + '/');
  return bk_qty;
}

///< load bk to buffer
bool    load_bk(char *dst, const uint32_t bk_no) {
  // load_bk(datfarm, FOFF[COUNT.bk].fileno, FOFF[COUNT.bk].offset)
  // load_bk(DATFARM_T &datfarm, const uint32_t fileno, const uint32_t offset)
    uint32_t sig, size;
    uint32_t fileno=FOFF[bk_no].fileno, offset=FOFF[bk_no].offset;
    if (!datfarm->read(fileno, offset-8, sizeof(sig), &sig))
      return b_error("Can't read bk signature.");
    if (sig != BK_SIGN)
      return b_error("Bad block signature found: " + ptr2hex(string_view((char *) &sig, sizeof(sig))));
    if (!datfarm->read(fileno, offset-4, sizeof(size), &size))
      return b_error("Can't read bk size.");
    if (size > MAX_BK_SIZE)
      return b_error("Block too big: " + to_string(size));
    return datfarm->read(fileno, offset, size, dst);
}

bool    stdin_bk(char *dst, const uint32_t bk_no) {
  string line;

  getline(cin, line);
  if (!line.empty()) {
    auto line_len = line.length();
    if (line_len > (MAX_BK_SIZE << 1))
      return b_error("Bk too big: " + to_string(bk_no));
    auto done = hex2bytes(line, dst);
    if (done != (line_len >> 1))
      return b_error(to_string(done) + "/" + to_string(line_len >> 1) + " bytes converted");
    return true;
  } else
    return false;
}
