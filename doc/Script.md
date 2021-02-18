# Script cases (vout)
[RTFM](https://learnmeabitcoin.com/guide/scriptPubKey)

1. P2PK (type: [pubkey](https://learnmeabitcoin.com/guide/p2pk)):
  - 67 bytes (0..727)
     - 1 = x41 (use next 65 bytes; 01..4e)
	 - 65 - pubkey
	 - 1 = xAC (OP_CHECKSIG)
2. P2PKH (type: [pubkeyhash](https://learnmeabitcoin.com/guide/p2pkh)):
  - 25 bytes (728)
    - 1 - x76 (OP_DUP) - хез (dup input pubkey)
    - 1 - xA9 (OP_HASH160) - next is address
    - 1 - x14 (20 bytes of address)
    - 20 - RIPEMD-160 (2xSHA256+ripemd16 of input pubkey)
    - 1 - 88 (OP_EQUALVERIFY) - (input and addr must be equal)
    - 1 = xAC (OP_CHECKSIG)
  - 4026 bytes (tx #97260) - 25x + acac...
3. P2MS (type: ...):
  - 
4. P2SH (type: [scripthash](https://learnmeabitcoin.com/guide/p2sh)):
  - 23 bytes:
    - 1 - xA9 (OP_HASH160) - next is address
    - 1 - x14 (20 bytes of address)
    - 20 - RIPEMD-160 (2xSHA256+ripemd16 of input pubkey)
    - 1 - 87 (OP_EQUAL)
5. NULL_DATA (type: nulldata):
  - 38 bytes:
    - x6A - OP_RETURN
	- x24 - next 36 bytes
	- 36 - ?
6. misc
  - 22 bytes: witness_v0_keyhash
    - 1 - 00
    - 1 - x14 - next 20 bytes
    - 1 - ripemd-160 == address
  - 33 bytes (witness_v0_scripthash)
	+ 1 - 00
	+ 1 - x20 - next 32 bytes == hash?
