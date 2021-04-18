#ifndef BKIDX_H
#define BKIDX_H

const uint32_t  MAX_BK_SIZE = 4 << 20;  // 4MB enough

size_t init_bkloader(const std::string, const std::string);
bool   load_bk(const uint32_t, char *);

#endif // BKIDX_H
