#include "bk/bk.h"
#include "bce.h"      // STAT, OPTS
#include "crypt/uintxxx.h"  // hash

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

BK_T::BK_T(string_view src, uint32_t bk_no) : height(bk_no), data(src) {
  UNIPTR_T uptr(data.cbegin());
  uptr.take_u8_ptr(sizeof(BK_HEAD_T)); // skip header
  auto tx_count = uptr.take_varuint();
  if (!(bk_no == BK_GLITCH[0] or bk_no == BK_GLITCH[1]))  // skip glitch blocks
    for (uint32_t i = 0; i < tx_count; i++, COUNT.tx++)
      txs.push_back(TX_T(uptr, i, bk_no));
  // Counters
  STAT.max_txs = max(STAT.max_txs, tx_count);
}

void BK_T::mk_hash(void) {
  hash256(data.begin(), sizeof(BK_HEADER_T), hash);
}

bool BK_T::parse(void) {
  // TODO: ? apply header
  if (OPTS.out)
    mk_hash();  // TODO: on demand/mt
  bool retvalue(true);
  for (auto it_tx = txs.begin(); it_tx != txs.end(); it_tx++) // TODO: m/t
    retvalue &= it_tx->parse();
  return retvalue;
}

bool BK_T::resolve(void) {
  bool retvalue(true);
  for (auto it_tx = txs.begin(); it_tx != txs.end(); it_tx++) // TODO: m/t
    retvalue &= it_tx->resolve();
  return retvalue;
}
