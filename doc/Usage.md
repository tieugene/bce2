# Usage

Application package consists from utilities:

- bce2 - main CLI utility
- btcbklocs - tool to prepafre locs-file (binary version)
- tools/btcbklocs.py - tool to prepafre locs-file (python3 version)
- btclocsviwe - helper to explore locs-file

## 1. Pre-run stage

As chain blocks are storing "in heap" bce2 requires to know block "locations" - what block in what blk*.dat in what position is stored.  
This information is not stored in blockchain itself but can be found in bitcoind helping LevelDB (now) database.  
To avoid extra job in run time a special utility prepares helping locs-file (block locations file) as sequence of 8-byte records `{datno:uin32,offset:uint32}`, where:
- record order no: == block height about; 0-based
- datno: dat-file number (e.g. `5` for `blk00005.dat`); 0-based
- offset: an offset of the block inside dat-file; 0-based

Let's make locs-file for **670k** blocks. Blockchain is stored in `$BTCDIR`.

1. run bitcoind:
   ```bash
   sudo systemctl start bitcoin
   ```
1. get block count (*to check how many blocks we have*):
   ```bash
   bitcoin-cli getblockcount
   ```
1. get 1-st 670k block hashes into `tmp.hex`:
   ```bash
   for i in ${seq 0 699999}; do blockchain-cli getblockhash $i >> tmp.hex; done
   ```
1. stop bitcoind:
   ```bash
   sudo systemctl stop bitcoin
   ```
1. fix blockchain permissions:
   ```
   sudo chmod -R o-w+rX $BTCDIR
   ```
1. copy LevelDB folder into somewhere (e.g. here):
   ```
   cp $BTSDIR/blocks/index .
   ```
1. make locs-file `bk.locs.670k.bin`:
   ```bash
   btcbklocs -x tmp.hex -i index -o bk.locs.670k.bin
   ```
   or (requires python3-plyvel):
   ```bash
   tools/btcbklocs.py -i tmp.hex index bk.locs.670k.bin
   ```

_Note: as block hashes are independent from blockchain nodes they can be scribed on any blockchain node (steps &numero; 1&hellip;4).  
But blk*.dat are unique for each bitcoind,
so LevelDB folder must be **exactly** from those $BTCDIR that will be used by bce2._

## 2. Run bce2

In short bce2 operates with 4 data "streams":

1. blocks: folder with blk*.dat where blocks live; input, mandatory
1. locs-file: single file with block "coordinates"; input, mandatory
1. k-v: interim storage of transactions (tx) and addresses (addr) hash&rarr;&numero; conversion; input/output, optional
1. output: text result (see "[Output](Output.md)"); output, optional

### CLI:

_(use `bce2 -h` to get this)_

- `-f <n>` : block starts from
- `-n <n>` : blocks to process
- `-d <path>` : *.dat folder
- `-k <path>` : folder for file-based key-value (optional)
- `-m` : use in-mem key-value
- `-o` : produces output results
- `-v <n>` : verbosity
- `locs-file` : mandatory argument

Combining -k, -m, -o, -v options produces special effects:

### Modes:

Walk and count depending on -c & -o:
- ~~c~~/~~o~~: btio (w/o uniq addrs)
- ~~c~~/o: btio and out blocks
- c/~~o~~: all (w/ uniq addrs)
- c/o: all and print results

### Use cases:
1. Speed walk (w/o calc, just test integrity)
2. + prints as table
3. ~~+ as json (univalue?)~~
4. Walk (w/ calc hashes and addresses; chk prev_hash)
5. + print as table
6. ~~+ as json (like getblock)~~
7. gen export (w/ k-v storage)

## X. Utility
```bash
#!/bin/sh
# get_bk.sh - get bk by height
bitcoin-cli getblock `bitcoin-cli getblockhash $1` 2 > $1.json
```
