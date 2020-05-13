#!/bin/env python3
"""
Get block info from LevelDB.
Input: bk no
Output: <bkno>.ldb

TODO: options:
-l - ldb dir
-f - from
-n - qty (default=1)
-d - dumprecords
-p - debug output
-c - create bk db (height:u32,file:u32,doffset:u32,uoffset:u32)
can be: height - 20 bit, file - 12 bit, offset - 28 bit => 2 x 32 bit
"""

import sys
from struct import unpack
import leveldb
from bitcoinrpc.authproxy import AuthServiceProxy

dbpath = "/mnt/sdb2/tmp/index"
db = None
BLOCK_HAVE_DATA = 8
BLOCK_HAVE_UNDO = 16
TOOBIGOFFSET = 256 << 20	# 256MB > any blk*.dat

def __get_varint(rec: bytes, pos: int = 0) -> tuple:
	"""
	Decode serialized varint.
	Powered by Mark Friedenbach's [VarInt](https://github.com/maaku/python-bitcoin/blob/master/bitcoin/serialize.py)
	@param rec - record itself
	@param pos: position to start from
	@return (value, bytes read)
	"""
	# print(rec)
	# print(type(rec))
	result = 0
	l = len(rec)
	while pos <= l:
		# limb = unpack(">B", rec[counter])	// unsigned char big endian
		limb = int(rec[pos])
		pos += 1
		result <<= 7
		result  |= limb & 0x7f
		if limb & 0x80:
			result += 1
		else:
			break
	# print("Result: %d, pos: %d" % (result, pos))
	return (result, pos)

def get_hash (height:int) -> bytes:
	""" Get bk hash by its height. """
	rpc_connection = AuthServiceProxy("http://login:password@127.0.0.1:8332")
	hash_s = rpc_connection.getblockhash(height)
	hash_b = bytes.fromhex(hash_s)[::-1]	# reverse == BE
	return hash_b

def get_ldbrec (hash_b: bytes) -> bytes:
	""" Get LevelDB's record of bk. Excluding bk head (last 80 bytes) """
	k = b'\x62' + hash_b
	v = db.Get(k)		# bytearray
	return bytes(v[:-80])

def dump_rec (height: int, rec: bytes):
	""" Record to file. """
	with open("ldb/%d.ldb" % height, "wb") as f:
		f.write(rec)
		f.close()

def print_rec (rec: bytes):
	"""
	Output record.
	(see bitoin-core, chain.h, class CDiskBlockIndex)

	"""
	v, i = __get_varint(rec)		# version
	print("Ver:\t%d" % v)
	v, i = __get_varint(rec, i)		# height
	print("Height:\t%d" % v)
	s, i = __get_varint(rec, i)		# status
	print("Status:\t%d" % s)
	v, i = __get_varint(rec, i)		# nTx
	print("nTx:\t%d" % v)
	if (s & (BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO)):
		v, i = __get_varint(rec, i)	# File no
		print("nFile:\t%d" % v)
		if (v > 2100):
			print("File # is too big:\t%d" % v, file=sys.stderr)
	if (s & BLOCK_HAVE_DATA):
		v, i = __get_varint(rec, i)	# blk*.dat offset
		print("DOff:\t%d" % v)
		if (v > TOOBIGOFFSET):
			print("Data offset is too big:\t%d" % v, file=sys.stderr)
	if (s & BLOCK_HAVE_UNDO):
		v, i = __get_varint(rec, i)	# rev*.dat offset
		print("UOff:\t%d" % v)
		if (v > TOOBIGOFFSET):
			print("Undo offset is too big:\t%d" % v, file=sys.stderr)
	if (i != len(rec)):
		print("Bytes decoded (%d) != record len (%d)." % (i, len(rec)), file=sys.stderr)

def main(bk_no):
	global db
	db = leveldb.LevelDB(dbpath)	# 0. prepare
	hash_b = get_hash(bk_no)	# 1. get bk hash
	rec = get_ldbrec(hash_b)	# 2. get ldb rec
	# r = decode_rec(rec)		# 3. decode it
	# write_db
	dump_rec(bk_no, rec)		# 3. dump rec
	print_rec(rec)			# 4. debut print

if __name__ == '__main__':
	if (len(sys.argv) != 2):
		print("Usage: %s <bk_no>" % sys.argv[0])
	else:
		main(int(sys.argv[1]))
