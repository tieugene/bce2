# Usage

Application package consists of CLI utilities:

- bce2 - main tool
- btcbklocs - utility to prepare locs-file (binary version)
- tools/btcbklocs.py - utility to prepare locs-file (python3 version)
- btclocsview - helper to explore locs-file

In short bce2 operates with 3 data "streams":

1. blocks *(input, required)*:
   - directly from blockchain using:
     - folder with blk*.dat where blocks live
     - [locs-file](MkLocs.md): single file with block "coordinates" in *.dat
   - from `bitcoind` hex output using `bitcoin-cli getblock <hash> 0`
1. k-v *(input/output, optional)*: interim storage of transactions (tx) and addresses (addr) hash&rarr;&numero; conversion
1. output *(output (stdout), optional)*: text results (see "[Output](Output.md)")

Additional output (stderr) is logging if `-v` option used.

## Options

Run options can be set using [config](bce2.cfg.5.adoc) file and/or [CLI](bce2.1.adoc) (command line interface).  
_Note: CLI overwrites config values._

### Modes:

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


### Verbosity:

1. statistic counters - bks, txs, vins, vouts, addrs found, uniq addrs, RAM used, timer
2. +misc summaries
3. not implementd

### Tunings:

`-t` usage described in extra [documentation](Tuning.md).
