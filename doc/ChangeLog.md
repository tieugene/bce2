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
     - add "2skip" (bk (20) + tx.bkno (10) + vout (12) (uint64?)) as file
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

## Timer:
(w/o addresses)
- 200k - 

## Usual:
- [Stat](bitinfocharts.com/ru/bitcoin/)
- [Bitcoin API](https://www.blockchain.com/api/blockchain_api)
- Use Blochain Explorer API for test addrs (https://www.blockchain.com/api)
- [jq](https://stedolan.github.io/jq/)

## Kyotocabinet backends:
- DB::PolyDB => [kyotocabinet::HashDB](https://dbmx.net//kyotocabinet/api/classkyotocabinet_1_1HashDB.html) ('kch')
- try: kc.occupy()
- try: on-memory [kyotocabinet::](https://dbmx.net//kyotocabinet/api/index.html):
  - [ProtoHashDB](https://dbmx.net//kyotocabinet/api/namespacekyotocabinet.html#a68826fcd903705a08001af0180d713e6) -- ('-') hash database based on std::unordered_map
  - [StashDB](https://dbmx.net//kyotocabinet/api/classkyotocabinet_1_1StashDB.html) -- (':') economical hash database for cache.
  - [CacheDB](https://dbmx.net//kyotocabinet/api/classkyotocabinet_1_1CacheDB.html) -- ('*') hash database for cache with LRU deletion
  - [GrassDB](https://dbmx.net//kyotocabinet/api/namespacekyotocabinet.html#ab08bf5f25c36e8e6b06e7531b047acc7) -- ('%') tree database for cache in order

----

## Order:
1. get bk's hashes (bash, py):
  - in: int range
  - out: X x 64-char lines
2. get bk's positions:
  - in: #1
  - out: height=>file+offset[+hash]
3. get COPY lines
  - in: #2
  - out - 1 x txt
4. sort vins:
  - in: #3 | grep ^i
  - out: sorted ^i (by tx>vout; "external sort": sort -T -S --compress-program)
5. merge vout+vin
  - in: #3, 4
  - out: ^o (w/ or w/o vin)
6. load into DB

## Resurces:
- [blk* structure]("https://en.bitcoin.it/wiki/Bitcoin_Core_0.11_(ch_2):_Data_Storage"):
  - blocks/blk*.dat - main data
  - blocks/index/* - where blocks are
  - blocks/rev*.dat - undos (?)
- better [blk* structure](https://learnmeabitcoin.com/guide/blkdat)

## output:
table:str = b/t/a/d--tab--data

* b (blocks):
  * b_id:int - PK (= block height)
  * b_time:timestamp - timestampt (= block time)
* t (transactions):
  * t_id.bigint - PK (= block.id << 32 || tx order no [<< 16])
  * hash:str[32] - tx hash (~~?~~)
  * b_id:int - FK(block.id)
* a (address):
  * a_id:bigint - PK (?last 8 bytes)
  * n:int - addresses qty
  * a_list:json - list of addresses (str[25])
* d (data):
  * ~~id:bigint~~ - PK (? = block.id << 32 || tx_order_no << 16 || vout no)
  * t_out_id:bigint - FK(transaction.PK) where this rec is vout
  * t_out_n:int - n of vout
  * [t_in_id]:bigint - FK(transaction.PK) where this rec is vin
  * ~~[t_in_n]:int~~
  * [a_id]:bigint - FK(address.PK) of vout recipient
  * satoshi:bigint
* x (data:address x-table):
  * d_id:bigint - FK(data.t_out_id)
  * d_no:int - FK(data.t_out_n)
  * a_id:bigint - FK(addresses.a_id)

## Use cases:
1. Speed walk (w/o calc, just test integrity)
2. + prints as table
3. ~~+ as json (univalue?)~~
4. Walk (w/ calc hashes and addresses; chk prev_hash)
5. + print as table
6. ~~+ as json (like getblock)~~
7. gen export (w/ k-v storage)

## Compare:
- get from bitcoind by py | cut extra | out
- + decode pubkeys

## LevelDB:
(CBlockTreeDB, src/txdb.h)
- Path: blocks/index/*
- Key: 'b' + 32-byte block hash -> block index record.
- Record (88 bytes; BE:
  * 80 - block header (bytes 8..88)
  * 4 - height (int)
  * 4 - The number of transactions.
  * ? - To what extent this block is validated.
  * 8 - In which file (int), and where in that file (int), the block data is stored.
  * 8 - In which file (int), and where in that file (int), the undo data is stored.

E.g.

- bk #0 (88):
  - 0x89 0xFE 0x04 - version
  - 0x00 - nHeight
  - 0x0B - nStatus	// have data & !undo
  - 0x01 - nTx (1)
  - 0x00 - nFile (0)
  - 0x08 - DOffset (8, after sig_size)
- bk #1 (90):
  - 0x89 0xFE 0x04 - version
  - 0x01 - vHeight (1)
  - 0x1D - vStatus	// have data & undo
  - 0x01 - vTx (1)
  - 0x00 - vFile (0)
  - 0x81 0x2D == 0x012D - vDatOffset (after sig_size)
  - 0x08 - vUndoOffset
- bk #120000 (95):
  - 0x89 0xFE 0x04 - version
  - 0x86 0xA8 0x40 - vHeight (120000)
  - 0x1D - vStatus	// have data & undo
  - 0x38 - vTx (56)
  - 0x01 - vFile (1)
  - 0x82 0xB1 0x00 - vDatOffset
  - 0x80 0x92 0x40 - vUndoOffset
- bk #629999 (100):
  - 0x8A 0xCC 0x14 - version
  - 0xA5 0xB8 0x6F - vHeight (629999)
  - 0x80 0x1D - vStatus
  - 0x92 0x31 - vTx (2481)
  - 0x8F 0x16 - vFile (2069)
  - 0x9F 0xC6 0xA2 0x14 - vDatOffset
  - 0x83 0xA7 0xCC 0x77 - vUndoOffset

* [python-verint](https://github.com/fmoo/python-varint) - wrong
* [varints](https://github.com/bright-tools/varints) - wrong (!dlugosz)
* [python-bitcoin](https://github.com/maaku/python-bitcoin) - the best (serialize.VarInt)
  * VarInt(<int>).serialize() -> bytes
  * VarInt().deserialize()

----
Note: tune = builin cache:
- &oplus; soft
- &#8860; inmem
- &ominus; too complex to handle
