# Install

## Build
- c++ compiler
- cmake
- c++ devel libs:
  - openssl
  - kyotocabinet
  - leveldb

`cmake && make && make install`

## Prepare

For pre-run stage (making locs-file; see "[Usage](Usage.md)"):

- working bitcoin node (bitcoind)
- bitcoin client (bitcoin-cli)
- leveldb runtime lib
- python3-plyvel (optional)

## Runtime
- runtime libs:
  - openssl
  - kyotocabinet
- bitcoin's data files (blk*.dat) with read permissions
- prepared locs-file (see "[Usage](Usage.md)")
- enough disk space for:
  - output data
  - key-value file-based storage (optional)
