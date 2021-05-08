# ToDo

## Hot

Clean up:

- kv.try()
- const *ptr
- ? uint8_t => char
- string_view
- exceptions
- stages: load | parse | resolve (and save) | print

## 210507 (400+410):
- [x] 20 raw: 1956
- [x] 20 -m : (2095+1325+&hellip;)
- [x] 28 raw: 2052

### .kch steps

n  | kbk | tx  | uaddr
---|----:|----:|------
21 | 150 |   2 |   2
23 | 200 |   8 |   7
24 | 250 |  21 |  16
25 | 300 |  38 |  36
26 | 350 |  64 |  70
27 | 400 | 112 | 129
28 | 450 | 191 | 218
-- | 500 | 284 | 347
29 | 600 | 466 |*600*

## 210415

- \#54 tkrzw (brew: mremap() absent)
- [x] \#56 BcGetHex() (from `bitcoin-cli |`)
- \#57 string_view
+ \#58 BcGetJson()
+ \#59 [unit]tests
+ \#61 moduled (pipe process)
+ \#51 multithreading:
  + print()
  + kv.add()/getoradd()
  + tx.calc_hash()
+ \#60 k-v: bulk save (tkrzw::DBM::SetMulti()); +bk qty
+ \#15 -m: +write2disk()

## Future

+ print_sql()
+ \#38 split: tx.kv=tx.history+utxo.kv
+ tx.id &= 128-bit => uint_128
+ multisig => separate k-v | skip
+ cache k-v (2-nd order for old)
