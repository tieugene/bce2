## Plan:
1. +Alpha walk (200506):
  - +light parse bk, tx, vin, vout
2. Correct walk:
  - +refactoring source (split by modules)
  - ?outputs (1. bk, 2. tx)
  - stop file on EOF
  - options (from, qty, noout, -v, <files>) // getopt, getoptpp, boost::program_options
  - debug outputs
  - tx.hash
3. Scripts:
  - decode addr
  - outputs (3. vout)
4. K-v
  - tx
  - addr
  - outputs (4. addr, 5. vin, 6. x)
5. chk bk
6. Mempool

## coinbase
- txid = 0x00....00
- vout = 0xFFFFFFFF
[Pruf](https://learnmeabitcoin.com/guide/coinbase-transaction)

## misc

need - params, bc container

commands:

- info/check
- export
params (export):
-f[rom] block
-q[ty] blocks
-k[eep]
intermediates:
- tx hashes
- address hashes
== each block:
- assign sig+len+header
- get txins; for each:
-- assign ver
-- get vcount; for each:
--- assign txid
--- assign vout
--- get ssize
--- assign script
--- assign seq
-- get vcount; for each:
--- assign satoshi
--- get ssize
--- assign script
-- assign locktime

## find:
- bk height - check by p_hash?
- calc block hash (2 x sha256 of bk.ver..bk.nonce included; print in reverse)
- ~~calc tx hash~~ (2 x sha256 of tx.ver..tx.locktime included; print in reverse)
