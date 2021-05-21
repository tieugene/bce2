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
  std::ifstream        *file;
  std::filesystem::path folder;
  size_t                qty;
  bool                  open(const size_t);
public:
  DATFARM_T(const size_t qty, const std::filesystem::path &folder)
    : folder(folder), qty(qty)
    { file = new std::ifstream[qty]; }
  bool                  read(const size_t, const size_t, const int, void *);
};

#endif // DATFARM_H
