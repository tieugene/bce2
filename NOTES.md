# Notes

## Todo:
1. &#9745; Alpha (_simple walk_): _200506_
  - &check;light parse bk, tx, vin, vout
1. &#9745; Pre-Beta (_tuning_): _200507_
  - &check;refactoring source (split by modules)
  - &check;output (bk, tx (w/o hash), vout)
  - &check;options (parse)
  - &check;options (use)
1. &#9744; Beta (_correct walk_):
  - &check;stop on EOF
  - multifile
  - tune debug levels
2. &#9744; RC1 (_hashes_):
  - bk.hash
  - tx.hash
  - output (tx.hash)
1. &#9744; RC2 (_scripts_):
  - decode addr
  - ~~outputs (addr)~~
1. &#9744; RC3 (_k-v_):
  - k-v:
     - tx
     - addr
  - output (addr, vin, x)
1. &#9744; Release (chk bk on longest chain)
1. &#9766; Extra (_mempool_)

## misc:
- [coinbase](https://learnmeabitcoin.com/guide/coinbase-transaction):
  - txid = 0x00....00
  - vout = 0xFFFFFFFF
- hash:
  - bk.hash = sha256(sha256(bk.header (ver..nonce)))
  - tx.hash = sha256(sha256(tx.ver..tx.locktime))
  - check(s:str):
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

```python
import hashlib
print(hashlib.sha256(hashlib.sha256(bytes.fromhex(s)).digest()).hexdigest())
```

## find:
- bk height - check by p_hash?

## Verbosity:
- 0 - errors only
- 1 - min:
  - options
  - file: name, size
  - summary
- 2 - mid
  - bk skipped:
  - bk: no, txs
- 3 - semi:
  - tx: no
- 4 - max:
  - vin:
  - vout:
  - addr:

## Test EOF:
- 0+1 - ok
- 0+3 - ok
- 3+5 - ok
- 0,\* - ok
- 3,\* - ok
- 200k,1
- 200k,*