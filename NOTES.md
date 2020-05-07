# Notes

## Plan:
1. &check;Alpha (simple walk): _200506_
  - &check;light parse bk, tx, vin, vout
1. &check;Tuning: _200507_
  - &check;refactoring source (split by modules)
  - &check;output (bk, tx (w/o hash), vout)
  - &check;options (parse)
  - &check;options (use num, quiet, debug, __file_):
1. Beta (correct walk):
  - stop file on EOF
  - _fileS_
  - bk.hash
  - tx.hash
  - output (tx (w/ hash))
1. RC1 (scripts):
  - decode addr
  - ~~outputs (addr)~~
1. RC3 (k-v storages):
  - k-v:
     - tx
     - addr
  - output (addr, vin, x)
1. Release (mempool)
1. Bugfix (chk bk on longest chain)

## misc:
- options libs:
  - [getopt](https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html) (unistd.h)
  - popt
  - [getoptpp](https://bitbucket.org/fudepan/getoptpp)
  - [getoptpp](https://github.com/cgloeckner/getoptpp)
  - [cxxopts](https://github.com/jarro2783/cxxopts)
  - boost::program_options
- commands:
  - info/check
  - export
- params (export):
  - f[rom] block
  - q[ty] blocks
  - k[eep]
- intermediates:
  - tx hashes
  - address hashes
- [coinbase](https://learnmeabitcoin.com/guide/coinbase-transaction):
  - txid = 0x00....00
  - vout = 0xFFFFFFFF
- hash:
  - bk.hash = sha256(sha256(bk.header))
  - tx.hash = sha256(sha256(tx.ver..tx.locktime))
  - check(s:str):

```python
import hashlib
print(hashlib.sha256(hashlib.sha256(bytes.fromhex(s)).digest()).hexdigest())
```

## find:
- bk height - check by p_hash?
- calc block hash (2 x sha256 of bk.ver..bk.nonce included; print in reverse)
- ~~calc tx hash~~ (2 x sha256 of tx.ver..tx.locktime included; print in reverse)
