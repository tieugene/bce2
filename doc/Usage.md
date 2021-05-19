# Usage

Application package consists of CLI utilities:

- bce2 - main tool
- btcbklocs - utility to prepare locs-file (binary version)
- tools/btcbklocs.py - utility to prepare locs-file (python3 version)
- btclocsview - helper to explore locs-file

## 1. Run bce2

In short bce2 operates with 3 data "streams":

1. blocks *(input, required)*:
   - directly from blockchain using:
     - folder with blk*.dat where blocks live
     - locs-file: single file with block "coordinates" in *.dat
   - from `bitcoind` hex output using `bitcoin-cli getblock <hash> 0`
1. k-v *(input/output, optional)*: interim storage of transactions (tx) and addresses (addr) hash&rarr;&numero; conversion
1. output *(output (stdout), optional)*: text results (see "[Output](Output.md)")

Additional output (stderr) is logging if `-v` option used.

### Options

Run options can be set using [config](bce2.cfg.5.adoc) file and/or [CLI](bce2.1.adoc) (command line interface).  
_Note: CLI overwrites config values._  
Combining `-e`, `-o` and `-v` options requires additional explanation:

-e   | -o| -v| Usage
-----|:-:|:-:|-------
none | - | - | Fast walk quietly (test blockchain parsing)
none | - | + | Fast walk with log (count bk, tx, vins, vouts, addrs)
none | + | - | Explore blockchain (debugging)
none | + | + | Explore blockchain with log
k-v  | - | - | Full walk quietly (test +k-v)
k-v  | - | + | Full walk with log (count +uniq addresses)
k-v  | + | - | Main job quietly
k-v  | + | + | Main job with log

Verbosity:
- 1: stat counts - bks, txs, vins, vouts, addrs found, uniq addrs, RAM used, timer
- 2: +misc summaries

## 2. Pre-run stage

Direct processing blockchain requires to know block "locations" - what block in what blk*.dat at what position is stored.  
To achieve this a special utility prepares helping locs-file (block **loc**ation**s** file) from blockchain LevelDB database.

Let's make locs-file for **670k** blocks. Blockchain is stored in `$BTCDIR`.

1. run bitcoind:

   ```bash
   sudo systemctl start bitcoin
   ```

1. get block count (*to check how many blocks we have*):

   ```bash
   bitcoin-cli getblockcount
   ```

1. get 1-st 670k block hashes into `tmp.hex` (filename is not matter):

   ```bash
   (for i in `seq 0 669999`; do bitcoin-cli getblockhash $i; done) > tmp.hex
   ```

1. stop bitcoind:

   ```bash
   sudo systemctl stop bitcoin
   ```

1. fix blockchain permissions:

   ```bash
   sudo chmod -R o-w+rX $BTCDIR
   ```

1. copy LevelDB folder into somewhere (e.g. here):

   ```bash
   cp $BTSDIR/blocks/index .
   ```

1. make locs-file `bk.locs.670k.bin` (filename is not matter):

   ```bash
   btcbklocs -i tmp.hex index bk.locs.670k.bin
   ```

   or (requires python3-plyvel):

   ```bash
   tools/btcbklocs.py -i tmp.hex index bk.locs.670k.bin
   ```

*Note: steps 1&hellip;4 can be done on any host having bitcoin-cli; steps 5&hellip;7 must be done _exactly_ from those \$BTCDIR that bce2 will use.*

## 3. K-V perfomance tuning

Perfomance of used k-v storages depends on reserved/stored records (tx and addrs) ratios in them. Records reserved during k-v creating.  
Best result is achiving when `reserverd` &sime; `stored`: too many `reserved` increase storage size and slow down starting job; too few `reserved` stay bottleneck when `reserverd` &Lt; `stored`.  
The solution is to rebuild k-v from time to time to aline `stored`/`reserverd` ratio, e.g. increasing `reserved` &times;2 each level.

Reserving recommended:

n  | kbk | tx  | uaddr
---|----:|----:|-----:
21 | 150 |   2 |   2
23 | 200 |   8 |   7
24 | 250 |  21 |  16
25 | 300 |  38 |  36
26 | 350 |  64 |  70
27 | 400 | 112 | 129
28 | 450 | 191 | 218
-- | 500 | 284 | 347
29 | 600 | 466 | 569
?? | 680 | 636 | 819

*Legend*:  
- `n`: power of 2 records to reserve (e.g. `28` == 2<sup>28</sup> == 256M records)  
- `kbk`: kiloblocks  
- `tx`: million transactions  
- `uaddr`: million unique addresses

Rebuilding kyotocabinet k-v storages using CLI (e.g. tx):

```bash
# 1. make empty storage with 1G reserved records
kchashmgr create -bnum 1073741824 tx.kch~
# 2. copy records from old to new
kchashmgr dump tx.kch | kchashmgr load tx.kch~
# 3. rename new into old
mv tx.kch~ tx.kch
```
