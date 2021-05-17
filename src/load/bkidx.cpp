/*
 * locs- and *.dat files handler
 */
#include <iostream>
#include <filesystem>
#include <stdio.h>

#include "bkidx.h"
#include "datfarm.h"
#include "misc.h"

using namespace std;

const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE
FOFF_T    *FOFF;
DATFARM_T *datfarm;
char *line = nullptr;

/// Load file-offset file
size_t  load_fileoffsets(const filesystem::path &fn) {
  ifstream file (fn, ios::in|ios::binary|ios::ate);
  if (!file)            // 1. open
    return u32_error("File '" + fn.string() + "' opening failed", 0);
  auto data_size = file.tellg();
  if ((data_size < 0) or (data_size & 0x7) or (data_size > (8 << 20)))    // 2. chk filesize
    return u32_error("Wrong file size (<0 or != 8x or >8MB (1M bks)): " + fn.string() + "=" + to_string(data_size), 0);
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

size_t  init_bkloader(const std::filesystem::path datdir, const std::filesystem::path locsfile) {
  auto bk_qty = load_fileoffsets(locsfile);
  if (bk_qty)
    datfarm = new DATFARM_T(bk_qty, datdir);
  return bk_qty;
}

/// Load bk to buffer
bool    load_bk(u8_t *dst, const uint32_t bk_no) {
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

bool    stdin_bk(u8_t *dst, const uint32_t bk_no) {
  const int BUF_SIZE = (MAX_BK_SIZE << 1) + 3;  ///< bk + \n + \0 + reserved
  if (!line)
    line = new char[BUF_SIZE];

  if (std::fgets(line, BUF_SIZE, stdin)) {
    auto line_len = strlen(line);
    if (line_len < 2)
      return b_error("Hex-line of bk " + to_string(bk_no) + " too small: " + to_string(line_len));
    if (line_len >= (BUF_SIZE - 1))   ///< max_bk_size exceeded
      return b_error("Hex-line of bk " + to_string(bk_no) + " too big: " + to_string(line_len));
    if (line[line_len - 1] == '\n')
      line[--line_len] = '\0';   // rtrim line
    if (line_len & 1)
      return b_error("Hex-line of bk " + to_string(bk_no) + " has odd symbols: " + to_string(line_len));
    auto done = hex2bytes(string_view(line, line_len), dst);
    if (done != (line_len >> 1))  // FIXME: WARNING: comare int and unsigned
      return b_error(to_string(done) + "/" + to_string(line_len >> 1) + " bytes converted");
    return true;
  } else {
    return false;
  }
}
