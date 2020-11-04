# TODO
## 201103:
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
- cpto{begin_transaction()..end-transaction()} - no effect

### Test on:
CPU: i5-3210M 2.5..3.1 GHz
RAM: 8GB
Range: -f 310k -n 10k (bk00138..00155)
KV: in-mem/HDD

#### 150..200:
- unpack: 6"
- std -m: 277" (4+5+131+75+62)
- std: 499"
- cpto{beg/end_trans} -m: 4+6+157+79+65
- open(OAUTOTRAN|OTRYLOCK) -m: 200" (4+6+43+80+67)
- open(OAUTOTRAN|OTRYLOCK): 350"

#### 200..250 -m:
- unpack: 28"
- ordinary -m: 32+21+630+410+335 = 1428" = 20
- cpto{beg/end_trans}: 36+37+668+...

bk |   Tx   |   Ad   |  Tx@HDD  |  Ad@HDD  | hJob | Tx.ld | Ad.ld | mJob
---|--------|--------|----------|----------|------|-------|-------|------
 50|   50779|   50420|   9449472|   9449472| 1 |  |  |  |
100|  216571|  174696|  20574208|  19619840| 3 |  |  |  |
150| 1718395| 2337716| 102527840| 118508088|  |  |  |  |
200| 7316306| 6576581| 416010856| 321973800|  |  |  |  |
250|21490939|16176859|1209790304| 782787336|  |  |  |  |
300|38463550|35538489|2160256520|1712265136|  |  |  |  |

