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

class DBSTAMP_T {
private:
  std::filesystem::path dbpath;
  std::fstream file;
  bool infile = false;
  uint32_t data[3] = {0, 0, 0};
public:
  DBSTAMP_T(const std::filesystem::path &);
  bool check(void);
  bool update(void);
  bool close();
  uint32_t get_bk(void) {return data[0];}
  uint32_t get_tx(void) {return data[1];}
  uint32_t get_ad(void) {return data[2];}
};

/**
 * @brief Set up K-V storages
 * @return True on success
 */
bool set_cache(void);
/// Reset k-v storages
void stop_cache(void);

#endif // KV_H
