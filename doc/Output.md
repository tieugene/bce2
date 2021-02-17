# Output

Output data is plain-text sequence of CR-separated records of TAB-separated fields (TSV - tab separated values).  

Record templates:

- `b	id	'created'	'hash'`
- `t	id	b.id	hash`
- `i	ti.id	vout	t.id`
- `o	t.id	vout	$	a.id|\N`
- `a	id	"addr"|["addr",…]	qty`

## Explanation

Each record describe one object (block/transaction/vin/vout/address) and
contains tab-separated fields.
Type of described object marked with 1-st field:

1. `b` - block (bk):
   - id:int - bk height
   - created:yyyy-mm-dd HH\:MM\:SS - bk creation datetime
   - hash:hex[40] - bk hash as hex-string
1. `t` - transaction (tx):
   - id:int - tx order № (0-based)
   - b.id:int - bk height that tx belongs to
   - hash:hex[40] - tx hash as hex-string
1. `i` - input (vin):
   - ti.id:int - tx № which vout used
   - vout:int - vout no (inside its tx) that used as money source
   - t.id:int - tx № what the vin belongs to
1. `o` - output (vout):
   - t.id:int - tx № what the vout belongs to
   - vout:int - vout no inside its tx
   - satoshi:int - money
   - a.id - address № or `\N` if it is absent/empty/wrong/undecryptable
1. `a` - address (addr):
   - id:int - order №
   - addr:str|list - address[es] hash[es] as json string (`"<hash>"`) or list (`["<hash>", "<hash>"]`)
   - qty:int - addresses quantity (1 for single address, 2+ for multisig)

## Sample

```
b       9       '2009-01-09 06:54:39'   '000000008d9dc510f23c2657fc4f67bea30078cc05a90eb89e84cc475c080805'
t       9       9       0437cd7f8525ceed2324359c2d0ba26006d92d856a9c20fa0241106ee5a597c9
a       9       "12cbQLTFMXRnSzktFkuoG3eHoMeFtpTu3S"    1
o       9       0       5000000000      9
...
b       170     '2009-01-12 06:30:25'   '00000000d1145790a8694403d4063f323d499e655c83426834d4ce2f8dd4a2ee'
t       170     170     b1fea52486ce0c62bb442b530a3f0132b826c74e473d1f2c220bfa78111c5082
a       170     "1PSSGeFHDnKNxiEyFrD1wcEaHr9hrQDDWc"    1
o       170     0       5000000000      170
t       171     170     f4184fc596403b9d638783cf57adfe4c75c605f6356fbc91338530e9831e9e16
i       9       0       171
a       171     "1Q2TWHE3GMdB6BZKafqwxXtWAWgFt5Jvm3"    1
o       171     0       1000000000      171
o       171     1       4000000000      9
```

Legend:

- block №9 contains
  - 1 coinbase tx №9 that
    - out (№0) all money (BTC50) into addr №9 ("12cb&hellip;Tu3S")
- block №170 contains 2 tx:
  - 1-st (№170) is coinbase and
    - out (№0) all money (BTC50) into address №170 ("1PSS&hellip;DDWc")
  - 2-nd (№171) is ordinar and:
    - get 1 output (№0) from tx №9
    - out (№0) a piece (BTC10) to new addr №171 (1Q2T&hellip;Jvm3)
    - and another (№1) piece (BTC40) back to addr №9
