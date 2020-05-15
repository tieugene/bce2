# Notes

## Todo:
1. &#9745; Pre-Alpha (_simple walk_): _(200506)_
  - &check;light parse bk, tx, vin, vout
1. &#9745; Alpha (_tuning_): _200507_
  - &check;refactoring source (split by modules)
  - &check;output (bk, tx (w/o hash), vout)
  - &check;options (parse)
  - &check;options (use)
1. &#9745; Pre-Beta (_correct walk_): _(200508)_
  - &check;stop on EOF
  - &check;multifile
1. &#9745; Beta (_hashes_): _(200508)_
  - &check;bk.hash
  - &check;tx.hash
  - &check;output (tx.hash)
1. &#9745; RC1 (_scripts_): _(200510)_
  - &check;decode addr (pk, pkg)
  - &check;output (addr, x)
  - &check;compare bk hashes (py) - not in right order
1. &#9745; RC2 (_LevelDB_): _(200513)_
  - &check;bk: height->hashes (tools/get\_bk\_hashes.py)
  - &check;bk: hashes->file+offset (tools/get\_bk\_locs.py)
1. &#9744; RC3 (_k-v_):
  - &check;bk: load by height
  - &check;tx k-v
  - &check;addr k-v
  - &check;output: vin, addr, x
  - &check;trace state on error
  - &hellip;extra addresses (incl. multiaddress)
     - add "2skip" (bk + tx.bkno + vout (uint32?))
     - or get patch?
1. &#9744; Release:
  - merge vout+vin
  - bk: hash2file_offset.cpp
  - ubunto pkg
1. &#9766; Extra:
  - -k (continue)
  - 'mempool'
1. &#9744; Bugfixes:
  - debug levels
  - __chk bk &/| tx hashes_
  - _debug as json_
1. &#9744; Feature requests:

## KB:
- bk hash _allways_ starts w/ 0x00000000 (uint32 0)
- [coinbase](https://learnmeabitcoin.com/guide/coinbase-transaction):
  - txid = 0x00....00
  - vout = 0xFFFFFFFF
- hash:
  - bk.hash = sha256(sha256(bk.header (ver..nonce)))
  - tx.hash = sha256(sha256(tx.ver..tx.locktime))
  - addr.pkh2addr = ripe160(sha256())
  - hash160 2 addr: base58(\0+hash160+(sha256(sha256(0+hash160)))[:4])
- 1sts (bk no my/real):
  - vin: 170
  - p2pkh: 728
  - p2ms:
     - [1st 164467]()
     - [2nd 165224]()
     - [3rd 165227]()
     - [1-of-1 431077]()
     - [1-of-2 229517]()
     - [1-of-3 442241]()

## Idea:
- ids: id as material path
  - bk.id:uint32_t = height
  - tx.id:uint64_t = bk.id << 4 + tx.no << 2
  - vout.id:uint_64_t = tx.id | vout.no
  - ~~tx: no = bk.height << 12 | tx.no (in bk)~~

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
addr: 1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa
```

## misc:
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
- speed:
  - noout:
    - 100k - 7"
  out:
    - 119k - 100"

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
