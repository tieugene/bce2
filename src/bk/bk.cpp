#include "bce.h"      // STAT, OPTS
#include "crypt/uintxxx.h"  // hash
#include "bk/bk.h"

using namespace std;

static uint32_t BK_GLITCH[] = {91722, 91842};    // dup of 91880, 91812
/// Bk header (80 bytes)
struct  BK_HEAD_T {
  uint32_t    ver;
  uint256_t   p_hash;
  uint256_t   mroot;
  uint32_t    time;
  uint32_t    bits;
  uint32_t    nonce;
};

BK_T::BK_T(const char *src, const uint32_t size, uint32_t bk_no) : data(src), size(size), height(bk_no) {
  UNIPTR_T uptr(data);
  //uptr.take_u8_ptr(sizeof(BK_HEAD_T)); // skip header
  const BK_HEAD_T* head_ptr = static_cast<const BK_HEAD_T*> ((const void *) uptr.take_u8_ptr(sizeof (BK_HEAD_T)));
  time = head_ptr->time;
  auto tx_count = uptr.take_varuint();
  if (!(bk_no == BK_GLITCH[0] or bk_no == BK_GLITCH[1]))  // skip glitch blocks
    for (uint32_t i = 0; i < tx_count; i++)
      txs.push_back(make_unique<TX_T>(uptr, i, this));
  // Counters
  STAT.max_txs = max(STAT.max_txs, tx_count);
  // cerr << "+BK " << to_string(height) << endl;
}

BK_T::~BK_T() {
  //for (auto tx: txs)
  //  delete tx;
  delete []data; // PVS warning
  // cerr << "-BK " << to_string(height) << endl;
}

void BK_T::mk_hash(void) {
  hash256(data, sizeof(BK_HEAD_T), hash);
}

bool BK_T::parse(void) {
  if (OPTS.out)
    mk_hash();
  for (auto &tx : txs)
    if (!tx->parse())
      return b_error("Bk # " + to_string(height) + " parse error");
  return true;
}

bool BK_T::resolve(void) {  // FIXME: rollback
  bool retvalue(true);
  for (auto &tx : txs) {
    retvalue &= tx->resolve();
    if (!retvalue)
      return b_error("Bk # " + to_string(height) + " resolve error");
  }
  return retvalue;
}
