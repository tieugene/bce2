# Install

## Build
- cmake
- c++ compiler
- c++ devel libs:
  - openssl
  - leveldb
  - one/some/all from:
     - kyotocabinet
     - tkrzw
     - libdb

`cmake && make && make install`

## Prepare

For pre-run stage (making locs-file; see "[Usage](Usage.md)"):

- working bitcoin node (bitcoind)
- bitcoin client (bitcoin-cli)
- leveldb runtime lib or
- python3-plyvel (optional)

## Runtime
- runtime libs:
  - openssl
  - one from:
     - kyotocabinet
     - tkrzw
     - bdb (BerkeleyDB)
- For parsing:
  - Direct:
     - bitcoin's data files (*blk\*.dat*) with read permissions (for direct parsing)
     - prepared locs-file (see "[Usage](Usage.md)")
  - from bitcoind:
     - running bitcoind
     - bitcoin-cli
- enough disk space for:
  - output data
  - key-value file-based storage (optional)
