/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <map>
#include "kv/kc.h"
#ifdef USE_TK
#include "kv/tk.h"
#endif

enum KVNGIN_T {
  KVTYPE_NONE
  ,KVTYPE_KCFILE
  ,KVTYPE_KCMEM
#ifdef USE_TK
  ,KVTYPE_TKFILE
  ,KVTYPE_TKMEM
#endif
};

bool    set_cache(void);  ///< setup k-v storages
void    stop_cache(void); ///< reset k-v storages

#endif // KV_H
