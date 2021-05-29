/** Key-value storage */
#ifndef KV_H
#define KV_H

#include <map>
#ifdef USE_KC
#include "kv/kc.h"
#endif
#ifdef USE_TK
#include "kv/tk.h"
#endif
#ifdef USE_BDB
#include "kv/bdb.h"
#endif

/// K-V storage engine type
enum KVNGIN_T {
  KVTYPE_NONE
#ifdef USE_KC
  ,KVTYPE_KCFILE
  ,KVTYPE_KCMEM
#endif
#ifdef USE_TK
  ,KVTYPE_TKFILE
  ,KVTYPE_TKMEM
#endif
#ifdef USE_BDB
  ,KVTYPE_BDB
#endif
};

/**
 * @brief Set up K-V storages
 * @return True on success
 */
bool set_cache(void);
/// Reset k-v storages
void stop_cache(void);
/// Update integrity info
bool update_integrity(void);

#endif // KV_H
