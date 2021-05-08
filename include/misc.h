#ifndef MISC_H
#define MISC_H

#include "bce.h"

void    load_cfg(void);
bool    cli(int const, char *[]); ///< Handle CLI. Return 0 if error, argv's index of 1st filename on success.
long    memused(void);
std::string  ptr2hex(std::string_view data);
int     hex2bytes(const std::string&, char *);

#endif // MISC_H
