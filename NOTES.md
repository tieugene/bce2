# Notes

## Todo:
1. &#9745; Alpha (_simple walk_): _200506_
  - &check;light parse bk, tx, vin, vout
1. &#9745; Pre-Beta (_tuning_): _200507_
  - &check;refactoring source (split by modules)
  - &check;output (bk, tx (w/o hash), vout)
  - &check;options (parse)
  - &check;options (use)
1. &#9745; Beta (_correct walk_): _200508_
  - &check;stop on EOF
  - &check;multifile
1. &#9745; RC1 (_hashes_): _200508_
  - &check;bk.hash
  - &check;tx.hash
  - &check;output (tx.hash)
1. &#9744; RC2 (_scripts_):
  - decode addr
  - trace state on error
  - chk bk &/| tx hashes
  - debug levels
  - _debug as json_
1. &#9744; RC3 (_k-v_):
  - k-v:
     - tx
     - addr
  - output (vin, addr, x)
1. &#9744; Release (chk bk on longest chain)
1. &#9766; Extra (_mempool_)

## KB:
- 1"/dat
- bk hash _allways_ starts w/ 0x00000000 (uint32 0)
- [coinbase](https://learnmeabitcoin.com/guide/coinbase-transaction):
  - txid = 0x00....00
  - vout = 0xFFFFFFFF
- hash:
  - bk.hash = sha256(sha256(bk.header (ver..nonce)))
  - tx.hash = sha256(sha256(tx.ver..tx.locktime))
  - addr.pk2addr = ripe160(sha256())
  - hash160 2 addr: base58(\0+hash160+(sha256(sha256(0+hash160)))[:4])
- 1sts (bk no my/real):
  - vin: 170
  - p2pkh: 723/728
- blk volumes (bk/tx, +):
  - 0: 119968	435128
  - 1: 131231	754423
- out script:
  - &hellip;

## py scripts:

- hash256(s:str):

```python
import hashlib
print(hashlib.sha256(hashlib.sha256(bytes.fromhex(s)).digest()).hexdigest())
```

- hash160(s:str):

```
import hashlib
print(hashlib.new('ripemd160', hashlib.sha256(bytes.fromhex(s)).digest()).hexdigest())
```
Example (addr #[0](https://www.blockchain.com/btc/block/000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f)):

```
pk: 04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f
hash160: 62e907b15cbf27d5425399ebf6f0fb50ebb88f18
B4_b58: 0062e907b15cbf27d5425399ebf6f0fb50ebb88f189f2d5996
addr: 1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa
```

## find:
- bk height - check by p_hash?

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

## Verbosity:
- 0 - errors only
- 1 - min:
  - options
  - file: name, size
  - summary
- 2 - mid
  - bk no, txs
- 3 - semi:
  - bk: skipped
  - tx: no, vins, vouts
- 4 - max:
  - vin:
  - vout:
  - addr:

## Tests:
- 0+1 - ok
- 0+3 - ok
- 3+5 - ok
- 0,0 - ok
- 3,0 - ok
- 200k,1 - ok
- 200k,0 - ok