# bce
Blockchain export.
CLI utility to export blockchain data into plain text format

## Utility
```
#!/bin/sh
# get bk by height
bitcoin-cli getblock `bitcoin-cli getblockhash $1` 2 > $1.jso
```

## Modes:
Walk and count depending on -c & -o:
- ~~c~~/~~o~~: btio (w/o uniq addrs)
- ~~c~~/o: btio and out blocks
- c/~~o~~: all (w/ uniq addrs)
- c/o: all and print results

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
