/*
 * locs- and *.dat files handler
 */
#include <iostream>
#include <cstring> // strlen
#include <filesystem>
#include <stdio.h>

#include "misc.h"
#include "load/bkidx.h"
#include "load/datfarm.h"
#include "load/fasthex.h"

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
  file.seekg (0, ios::beg);
  auto tmp = reinterpret_cast<char *>(FOFF);
  file.read (tmp, data_size);
  file.close();
  return blocks;
}

size_t  init_bkloader(const std::filesystem::path &datdir, const std::filesystem::path &locsfile) {
  auto bk_qty = load_fileoffsets(locsfile);
  if (bk_qty)
    datfarm = new DATFARM_T(bk_qty, datdir);
  return bk_qty;
}

/// Load bk to buffer
string_view load_bk(const uint32_t bk_no) {
  uint32_t sig, size;
  uint32_t fileno=FOFF[bk_no].fileno, offset=FOFF[bk_no].offset;
  if (!datfarm->read(fileno, offset-8, sizeof(sig), &sig))
    return sv_error("Bk #" + to_string(bk_no) + ": Can't read bk signature.");
  if (sig != BK_SIGN)
    return sv_error("Bk #" + to_string(bk_no) + ": Bad block signature found: " + ptr2hex(string_view((char *) &sig, sizeof(sig))));
  if (!datfarm->read(fileno, offset-4, sizeof(size), &size))
    return sv_error("Bk #" + to_string(bk_no) + ": Can't read bk size.");
  if (size > MAX_BK_SIZE)
    return sv_error("Bk #" + to_string(bk_no) + ": Block too big: " + to_string(size));
  // simple
  char *buffer = new char[size];
  if (!datfarm->read(fileno, offset, size, buffer)) {
    delete []buffer;
    return sv_error("Bk #" + to_string(bk_no) + ": Cannot read block itself.");
  }
  return string_view(buffer, size);
  /* uniq
  unique_ptr<char[]> buffer = make_unique<char[]>(size);
  if (!datfarm->read(fileno, offset, size, buffer.get()))
    return sv_error("Bk #" + to_string(bk_no) + ": Cannot read block itself.");
  return string_view(buffer, size); // x
  */
}

/**
 * @brief Convert hex-string into bytes
 * @param src String to convert
 * @param dst Buffer for result
 * @return Bytes converted
 */
size_t hex2bytes(string_view src, char *const dst) {
  auto src_ptr = src.begin();
  char *dst_ptr;

  for (dst_ptr = dst; src_ptr < src.end(); src_ptr += 2, dst_ptr++)
    *dst_ptr = (hextoint(src_ptr[0]) << 4) | hextoint(src_ptr[1]);
  return dst_ptr - dst;
}

string_view stdin_bk(const uint32_t bk_no) {
  const int BUF_SIZE = (MAX_BK_SIZE << 1) + 3;  ///< bk + \n + \0 + reserved
  if (!line)
    line = new char[BUF_SIZE];

  if (std::fgets(line, BUF_SIZE, stdin)) {
    auto line_len = strlen(line);
    if (line_len < 2)
      return sv_error("Bk #" + to_string(bk_no) + ": Hex-line too small: " + to_string(line_len));
    if (line_len >= (BUF_SIZE - 1))   ///< max_bk_size exceeded
      return sv_error("Bk #" + to_string(bk_no) + ": Hex-line too big: " + to_string(line_len));
    if (line[line_len - 1] == '\n')
      line[--line_len] = '\0';   // rtrim line
    if (line_len & 1)
      return sv_error("Bk #" + to_string(bk_no) + ": Hex-line has odd symbols: " + to_string(line_len));
    auto buffer_len = line_len / 2;
    char *buffer = new char[buffer_len];
    auto done = hex2bytes(string_view(line, line_len), buffer);
    if (done != buffer_len) {
      delete []buffer;
      return sv_error(to_string(done) + "/" + to_string(buffer_len) + " bytes converted");
    }
    return string_view(buffer, buffer_len);
  } else
    return string_view();
}
