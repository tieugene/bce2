#ifndef DATFARM_H
#define DATFARM_H

#include <fstream>

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

#endif // DATFARM_H
