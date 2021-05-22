#ifndef MISC_H
#define MISC_H

#include "bce.h"

/// Load options from config and CLI
bool        load_opts(int argc, char *[]);
/// Current resident memory usage (KB)
long        memused(void);
/// Convert bytes into hex string
std::string ptr2hex(std::string_view data);

#endif // MISC_H
