# Usage

Application package consists of CLI utilities:

- bce2 - main tool
- btcbklocs - utility to prepare locs-file (binary version)
- tools/btcbklocs.py - utility to prepare locs-file (python3 version)
- btclocsview - helper to explore locs-file

## 1. Pre-run stage

As chain blocks are storing "in heap" bce2 requires to know block "locations" - what block in what blk*.dat at what position is stored.  
This information is not stored in blockchain itself but can be found in bitcoind helping LevelDB (now) database.  
To avoid extra job in run time a special utility prepares helping locs-file (block **loc**ation**s** file) as sequence of 8-byte records `{datno:uin32,offset:uint32}`, where:
- record order no: == block height about; 0-based
- datno: dat-file number (e.g. `5` for `blk00005.dat`); 0-based
- offset: an offset of the block inside dat-file in bytes; 0-based

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
   (for i in ${seq 0 699999}; do blockchain-cli getblockhash $i; done) > tmp.hex
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
1. output: text result (see "[Output](Output.md)"); output (stdout), optional

Additional output (stderr) is logging if `-v` option used.

### CLI explanation:

_(use `bce2 -h` to get this)_

Options:

- `-f <n>` : block to start from
- `-n <n>` : blocks to process
- `-m` : use in-mem key-value
- `-k <path>` : folder for file-based key-value
- `-o` : produces output results (to stdout); printing format depens on k-v: 'debug' if no one from or 'main' (see "[Output](Output.md)") if any/both of `-m`/`-k` set
- `-v <n>` : verbosity (to stderr) - show processed blocks, transactions, vins, vouts, addresses and some other info (depends on k-v set and vebosity level), each 1000 blocks and summary

Mandatory:

- `locs-file` : mandatory argument
- `-d <path>` : *.dat folder

As all options excepting `-m` and `-k` are more or less trivial as combining `-m`/`-k` requires additional explanation:

1. *None*: without any k-v; just fast walk throught blockchain counting bk/tx/vins/vouts/addrs:
   - simple test without `-v` and `-o`
   - usual blockchain info with `-v`
   - explore blockchain for debugging with `-o`
   - &oplus; fastest start, &oplus; fastest run, &oplus; saves RAM, &ominus; for testing only
1. *`-m`* only: in-memory k-v; like №1 + counts uniq addresses:
   - has no sense w/o `-v` or `-o` (excepting testing)
   - full blockchain info (including uniq addresses) with `-v`
   - produces main output with `-o`
   - &oplus; fast start,
     &oplus; fast run,
     &ominus; eat RAM,
     &ominus; not saves k-v =>
     &ominus; *exactly* from bk 0
1. *`-k`* only: file-based k-v; like №2 but:
   - &oplus; fast start,
     &ominus; *slowest* run,
     &oplus; saves RAM,
     &oplus; saves k-v =>
     &oplus; starts from any bk
1. *`-m -k`*: use both k-v storage - a) load k-v from file into mem on start (`-k`), b) process data using in-mem k-v (`-m`) and c) saves k-v back to file (`-k`):
   - &ominus; *slow* start and stop,
     &ominus; fast run,
     &ominus; eat RAM,
     &oplus; saves k-v =>
     &oplus; starts from any bk

## x. Utility
```bash
#!/bin/sh
# get_bk.sh - get bk by height
bitcoin-cli getblock `bitcoin-cli getblockhash $1` 2 > $1.json
```
