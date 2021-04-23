#ifndef BKIDX_H
#define BKIDX_H

const uint32_t  MAX_BK_SIZE = 4 << 20;  // 4MB enough

size_t init_bkloader(const std::string, const std::string);
bool   load_bk(char *, const uint32_t);
bool   stdin_bk(char *, const uint32_t);

#endif // BKIDX_H
