# Notes

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

## Usual:
- [Stat](bitinfocharts.com/ru/bitcoin/)
- [Bitcoin API](https://www.blockchain.com/api/blockchain_api)
- Use Blochain Explorer [API](https://www.blockchain.com/api) for test addrs
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
- blk* structure: [RTFM1]("https://en.bitcoin.it/wiki/Bitcoin_Core_0.11_(ch_2):_Data_Storage"), [RTFM2](https://learnmeabitcoin.com/guide/blkdat)

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

----
Note: tune = builtin cache:
- &oplus; soft
- &#8860; inmem
- &ominus; too complex to handle
