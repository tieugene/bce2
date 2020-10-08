# TODO
201008:
- DB::PolyDB => [kyotocabinet::HashDB](https://dbmx.net//kyotocabinet/api/classkyotocabinet_1_1HashDB.html)
- try: kc.occupy()
- try: on-memory kyotocabinet::*:
  - [ProtoHashDB](https://dbmx.net//kyotocabinet/api/namespacekyotocabinet.html#a68826fcd903705a08001af0180d713e6) -- hash database based on std::unordered_map
  - [StashDB](https://dbmx.net//kyotocabinet/api/classkyotocabinet_1_1StashDB.html) -- economical hash database for cache.
  - [CacheDB](https://dbmx.net//kyotocabinet/api/classkyotocabinet_1_1CacheDB.html) -- hash database for cache with LRU deletion
  - [GrassDB](https://dbmx.net//kyotocabinet/api/namespacekyotocabinet.html#ab08bf5f25c36e8e6b06e7531b047acc7) -- tree database for cache in order
- PR.host002: create LXC F32 + shared btc dir

## Done:
- 201008:
  - ~~memused()~~

## Try
- man join
- [RocksDB](https://github.com/twmht/python-rocksdb)
- [KvKit](https://github.com/coleifer/kvkit)
