/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <set>
#include "kv/kc.h"
#ifdef USE_TK
#include "kv/tk.h"
#endif

const std::set<std::string> kvnames = {"kch", "kcs", "tkh", "tkt"};

bool    set_cache(void);  ///< setup k-v storages
void    stop_cache(void); ///< reset k-v storages

#endif // KV_H
