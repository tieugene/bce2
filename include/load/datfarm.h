#ifndef DATFARM_H
#define DATFARM_H

#include <fstream>
#include <filesystem>

/// File-offset array
struct  FOFF_T {
  uint32_t  fileno, offset;
};

/// Representation of blk*.dat, opening on demand
class   DATFARM_T {
private:
  std::ifstream        *file;     ///< opened file cash
  std::filesystem::path folder;   ///< base *.dat folder
  size_t                qty;
  bool                  open(const size_t);
public:
  DATFARM_T(const size_t qty, const std::filesystem::path &folder)
    : folder(folder), qty(qty)
    { file = new std::ifstream[qty]; }
  // TODO: allocate memory and return real bytes read
  bool                  read(const size_t, const size_t, const int, void *);
};

#endif // DATFARM_H
