#include <iostream>
#include <filesystem>

#include "bce.h"
#include "bkidx.h"

using namespace std;

FOFF_T   *FOFF;
const uint32_t  BK_SIGN = 0xD9B4BEF9;   // LE

bool        DATFARM_T::open(const size_t no) {
    if (no > qty) {
        cerr << "File # too big: " << no << endl;
        return false;
    }
    if (file[no].is_open())
        return true;
    ostringstream ss;
    ss << setw(5) << setfill('0') << no;
    string fn = folder + "blk" + ss.str() + ".dat";
    file[no].open(fn, ios::in|ios::binary);
    if (!file[no].is_open()) {
        cerr << "Can't open file " << fn << endl;
        return false;
    }
    return true;
}

bool        DATFARM_T::read(const size_t no, const size_t offset, const int size, void *dst)
{
    if (!open(no))
        return false;
    file[no].seekg(offset, file[no].beg);
    file[no].read(static_cast<char *>(dst), size);
    if (file[no].gcount() != size) {
        cerr << "Can't read " << size << " bytes from " << no << "." << offset << endl;
        return false;
    }
    return true;
}

size_t  load_fileoffsets(const char *fn)  ///< load file-offset file
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

bool    load_bk(DATFARM_T &datfarm, const uint32_t fileno, const uint32_t offset)       ///< load bk to buffer
{
    uint32_t sig, size;
    if (!datfarm.read(fileno, offset-8, sizeof(sig), &sig)) {
        cerr << "Can't read bk signature." << endl;
        return false;
    }
    if (sig != BK_SIGN) {
        cerr << "Block signature not found: " << hex << sig << endl;
        return false;
    }
    if (!datfarm.read(fileno, offset-4, sizeof(size), &size)) {
        cerr << "Can't read bk size." << endl;
        return false;
    }
    if (size > MAX_BK_SIZE) {
        cerr << "Block too big: " << size << endl;
        return false;
    }
    //BUFFER.end = BUFFER.beg + size;
    CUR_PTR.v_ptr = BUFFER.beg;
    return datfarm.read(fileno, offset, size, BUFFER.beg);
}

//fs::path sDatName = OPTS.bkdir + "/" + argv[file1idx];
//cerr << dir << TAB << file << endl;
//auto sDatPAth = dir.append(file);
//auto sDatPAth = std::filesystem::path(OPTS.bkdir, argv[file1idx]);
