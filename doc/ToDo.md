# ToDo

## 210415

- tkrzw
  - brew (mremap() absent)
- BcGetHex() (from `bitcoin-cli |`)
+ BcGetJson()
+ print_sql()
+ -m: +write2disk()
+ multithreading:
  + print()
  + kv.add()/getoradd()
  + &hellip;
  + tx.calc_hash()
+ k-v:
  + cmake
  + GetOrSet()
  + bk qty
  + bulk save (tkrzw::DBM::SetMulti())
+ [unit]tests
+ split: tx.kv=tx.history+utxo.kv

Note: memmem() and `no member named 'ru_maxrss' in 'rusage'` - must be __DARWIN_C_LEVEL >= __DARWIN_C_FULL

## Enhancements:

- Threading: (std::thread)
  - print
  - write-through tx/addr cache
- tx.id &= 128-bit => uint_128
- multisig => separate k-v | skip
- drop wrong ($0, !addr)
- cache k-v (2-nd order for old)
- semi-in-mem (tx | addr)

## Unordered:

### 201103:
- test count() => count
- occupy() (cpto, add, get)
- open(OAUTOTRAN) - each updating operation is performed in implicit transaction
- open(OTRYLOCK) - locking is performed without blocking
- cach+mem: OREADER-close-OWRITER
- another backend:
  - HashDB (.kch) => DirDB (.kcd)
  - leveldb
