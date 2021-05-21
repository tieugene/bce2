# Address

Address types:

P2…  | Name                  |sLen| Action  |dLen|kLen| type
-----|-----------------------|---:|---------|---:|---:|-----
PKu  | pubkey                | 65 | hash160 | 20 | 21 | u160
PKc  | pubkey                | 33 | hash160 | 20 | 21 | u160
PKH  | pubkeyhash            | 20 | cp      | 20 | 21 | u160
SH   | scripthash            | 20 | cp      | 20 | 21 | u160
WPKH | witness_v0_keyhash    | 20 | cp      | 20 | 21 | u160
WSH  | witness_v0_scripthash | 32 | cp      | 32 | 32 | u256
MS   | multisig              |×33/65|×hash160 |×20 |×20 |?vec<u160>

Legend:

- sLen: source bytes
- dLen: destination (stored) bytes
- kLen: k-v key bytes
- type: STL container type

## Classes

parse(u8_t *);
string to_string();	 // for printing; or to_json()?
vector<u8> to_key(); // k-v; ?cached
?resolve?

Variants:
- 1 universal class
- 1 virtual parent + 8 successors
