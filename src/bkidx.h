#ifndef BKIDX_H
#define BKIDX_H

const uint32_t  MAX_BK_SIZE = 4 << 20;  // 4MB enough

struct  FOFF_T      ///< files-offset array
    { uint32_t  fileno, offset; };

class   DATFARM_T   ///< represents blk*.dat, opening on demand
{
private:
    std::ifstream *file;
    std::string   folder;
    size_t        qty;
    bool          open(const size_t);
public:
    DATFARM_T(const size_t qty, const std::string &folder)
        : folder(folder), qty(qty)
        { file = new std::ifstream[qty]; }
    bool        read(const size_t, const size_t, const int, void *);
};

size_t  load_fileoffsets(const char *);  ///< load file-offset file
bool    load_bk(DATFARM_T &, const uint32_t, const uint32_t);       ///< load bk to buffer

extern FOFF_T   *FOFF;

#endif // BKIDX_H
