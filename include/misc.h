#ifndef MISC_H
#define MISC_H

#include "bce.h"

const string FROM_STDIN = "-";

bool        cli(int const, char *[]); ///< Handle CLI. Return 0 if error, argv's index of 1st filename on success.
long        memused(void);
string      ptr2hex(string_view data);
int         hex2bytes(const string&, char *);

#endif // MISC_H
