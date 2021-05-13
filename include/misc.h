#ifndef MISC_H
#define MISC_H

#include "bce.h"

bool    load_opts(int argc, char *[]);
long    memused(void);
std::string  ptr2hex(std::string_view data);
int     hex2bytes(std::string_view, u8_t *const);

#endif // MISC_H
