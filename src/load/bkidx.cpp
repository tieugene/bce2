/*
 * locs- and *.dat files handler
 */
#include <iostream>
#include <filesystem>
//#include <cstdint>
//#include <fstream>

#include "bkidx.h"
#include "datfarm.h"

using namespace std;

FOFF_T    *FOFF;
DATFARM_T *datfarm;
const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE

size_t  load_fileoffsets(const string fn)  ///< load file-offset file
{
    ifstream file (fn, ios::in|ios::binary|ios::ate);
    if (!file) {            // 1. open
        cerr << "File '" << fn << "' opening failed" << endl;
        return 0;
    }
    auto data_size = file.tellg();
    if ((data_size < 0) or (data_size & 0x7) or (data_size > (8 << 20)))    // 2. chk filesize
    {
        cerr << "Wrong file size (<0 or != 8x or >8MB (1M bks)): " << fn << "=" << data_size << endl;
        return 0;
    }
    auto blocks = size_t(data_size >> 3);
    FOFF = new FOFF_T[blocks];
    if (!FOFF) {
        cerr << "Can't allocate mem for file-offset list." << endl;
        return 0;
    }
    file.seekg (0, ios::beg);
    auto tmp = reinterpret_cast<char *>(FOFF);
    file.read (tmp, data_size);
    file.close();
    return blocks;
}

size_t  init_bkloader(const std::string datdir, const std::string locsfile)
{
  auto bk_qty = load_fileoffsets(locsfile);
  if (bk_qty)
    datfarm = new DATFARM_T(bk_qty, (datdir.back() == '/') ? datdir : datdir + '/');
  return bk_qty;
}

bool    load_bk(const uint32_t bk_no, char *dst)       ///< load bk to buffer
{
  // load_bk(datfarm, FOFF[COUNT.bk].fileno, FOFF[COUNT.bk].offset)
  // load_bk(DATFARM_T &datfarm, const uint32_t fileno, const uint32_t offset)
    uint32_t sig, size;
    uint32_t fileno=FOFF[bk_no].fileno, offset=FOFF[bk_no].offset;
    if (!datfarm->read(fileno, offset-8, sizeof(sig), &sig)) {
        cerr << "Can't read bk signature." << endl;
        return false;
    }
    if (sig != BK_SIGN) {
        cerr << "Block signature not found: " << hex << sig << endl;
        return false;
    }
    if (!datfarm->read(fileno, offset-4, sizeof(size), &size)) {
        cerr << "Can't read bk size." << endl;
        return false;
    }
    if (size > MAX_BK_SIZE) {
        cerr << "Block too big: " << size << endl;
        return false;
    }
    return datfarm->read(fileno, offset, size, dst);
}

//fs::path sDatName = OPTS.bkdir + "/" + argv[file1idx];
//cerr << dir << TAB << file << endl;
//auto sDatPAth = dir.append(file);
//auto sDatPAth = std::filesystem::path(OPTS.bkdir, argv[file1idx]);
