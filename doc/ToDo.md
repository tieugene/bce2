# ToDo

## Hot

1. separate bk source
1. common bk source
1. += hex

Separate:
- init(locs path, blocks path):
   load_fileoffsets(), datfarm(), load_bk()
- seek(int)
- get/load(dst)

## 210415

- \#54 tkrzw (brew: mremap() absent)
- \#56 BcGetHex() (from `bitcoin-cli |`)
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
