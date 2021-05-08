/** Key-value storage */
#ifndef KV_H
#define KV_H

#include "kv/kc.h"
#ifdef TKRZW
#include "kv/tk.h"
#endif

bool    set_cache(void);  ///< setup k-v storages
void    stop_cache(void); ///< reset k-v storages

#endif // KV_H
