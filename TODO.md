# TODO
## 201103:
- begin_transaction()..end-transaction() (cpto)
- count() => count
- occupy() (cpto, add, get)
- open(OAUTOTRAN) - each updating operation is performed in implicit transaction
- open(OTRYLOCK) - locking is performed without blocking
- cach+mem: OREADER-close-OWRITER
- tune bnum, msiz (kchashtest)
- another backend:
  - HashDB (.kch) => DirDB (.kcd)
  - leveldb
- threaded print (std::thread)
- write-through cache (threaded)

### Done
- status 
- sync && close?, info? check?
- slow start &larr; db.sync; db.close()

### Test on:
CPU: i5-3210M 2.5..3.1 GHz
RAM: 8GB
Range: -f 310k -n 10k (bk00138..00155)
KV: in-mem/HDD

#### 50..250:
broken

#### 300k:
- unpack: 3'20"
- job (tx.ld+ad.ld+job):
   - 328+325+1763 = 42'
   - 430+324+

bk |   Tx   |   Ad   |  Tx@HDD  |  Ad@HDD  | hJob | Tx.ld | Ad.ld | mJob
---|--------|--------|----------|----------|------|-------|-------|------
 50|   50779|   50420|   9449472|   9449472| 1 |  |  |  |
100|  216571|  174696|  20574208|  19619840| 3 |  |  |  |
150| 1718395| 2337716| 102527840| 118508088|  |  |  |  |
200| 7316306| 6576581| 416010856| 321973800|  |  |  |  |
250|21490939|16176859|1209790304| 782787336|  |  |  |  |
300|38463550|35538489|2160256520|1712265136|  |  |  |  |

