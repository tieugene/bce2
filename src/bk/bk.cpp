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

BK_T::BK_T(std::unique_ptr<char[]> src, uint32_t bk_no) : data(move(src)), height(bk_no) {
  UNIPTR_T uptr(data.get());
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

const string BK_T::err_prefix(void) {
  return "Bk # " + std::to_string(height) + ": ";
}

void BK_T::mk_hash(void) {
  hash256(data.get(), sizeof(BK_HEAD_T), hash);
}

bool BK_T::parse(void) {
  if (OPTS.out)
    mk_hash();
  for (auto &tx : txs)
    if (!tx->parse())
      return b_error(err_prefix() + "Parse error");
  return true;
}

bool BK_T::resolve(void) {
  bool retvalue(true);
  uint32_t qty(0);
  for (auto &tx : txs) {
    if (!(retvalue &= tx->resolve()))
      break;
    else
      qty++;
  }
  if (!retvalue) {
    v_error(err_prefix() + "Resolve oops.");
    rollback();  // TODO: 1st qty
  }
  return retvalue;
}

void BK_T::rollback(void) {
  bool retvalue(true);
  if (OPTS.verbose == DBG_MAX)
    v_error(err_prefix() + "Rolling back...");
  for (auto &tx : txs)
    retvalue &= tx->rollback();
  if (!retvalue)
    v_error(err_prefix() + "Rolling back oops.");
}
