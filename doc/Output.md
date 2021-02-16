# Output

Plaintext records:

- `b	id	'datetime'	'hash'`
- `t	id	b.id	hash`
- `i	t.id	vout	t.id`
- `o	t.id	vout	$`
- `a	id	"addr"|["addr",…]	qty`

table:str = b/t/a/d--tab--data

* b (blocks):
  * b_id:int - PK (= block height)
  * b_time:timestamp - timestampt (= block time)
* t (transactions):
  * t_id.bigint - PK (= block.id << 32 || tx order no [<< 16])
  * hash:str[32] - tx hash (~~?~~)
  * b_id:int - FK(block.id)
* a (address):
  * a_id:bigint - PK (?last 8 bytes)
  * n:int - addresses qty
  * a_list:json - list of addresses (str[25])
* d (data):
  * ~~id:bigint~~ - PK (? = block.id << 32 || tx_order_no << 16 || vout no)
  * t_out_id:bigint - FK(transaction.PK) where this rec is vout
  * t_out_n:int - n of vout
  * [t_in_id]:bigint - FK(transaction.PK) where this rec is vin
  * ~~[t_in_n]:int~~
  * [a_id]:bigint - FK(address.PK) of vout recipient
  * satoshi:bigint
* x (data:address x-table):
  * d_id:bigint - FK(data.t_out_id)
  * d_no:int - FK(data.t_out_n)
  * a_id:bigint - FK(addresses.a_id)
