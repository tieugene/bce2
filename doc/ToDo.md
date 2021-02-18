# ToDo

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
