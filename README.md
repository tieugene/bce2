# bce
Blockchain export.
CLI utility to export blockchain data into plain text format

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
- [blk* structure](https://en.bitcoin.it/wiki/Bitcoin_Core_0.11_(ch_2):_Data_Storage):
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
- 

## TODO:
Include bitcoin:
- uin256
- crypto/*
