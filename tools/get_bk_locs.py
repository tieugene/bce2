#!/bin/env python3
"""
Get block info from LevelDB.
Input: bk hashes
Output: bk file+offset (8 bytes per bk)
Result: 80..90"/630k, 

TODO: options:
-l - ldb dir
-f - from
-n - qty (default=1)
-d - dumprecords
-p - debug output
-c - create bk db (height:u32,file:u32,doffset:u32,uoffset:u32)
"""

import sys
from struct import pack
from collections import namedtuple
import leveldb

# consts
dbpath = "/mnt/sdb2/tmp/index"
BLOCK_HAVE_DATA = 8
BLOCK_HAVE_UNDO = 16
TOOBIGOFFSET = 256 << 20	# 256MB > any blk*.dat
# vars
LdbRec = namedtuple('LdbRec', ['v', 'h', 's', 't', 'f', 'd', 'u'])
db = None
TODUMP = False
TODEBUG = False

def eprint (s: str):
	print(s, file=sys.stderr)
	

def __get_varint(rec: bytes, pos: int = 0) -> tuple:
	"""
	Decode serialized varint.
	Powered by Mark Friedenbach's [VarInt](https://github.com/maaku/python-bitcoin/blob/master/bitcoin/serialize.py)
	@param rec - record itself
	@param pos: position to start from
	@return (value, bytes read)
	"""
	result = 0
	l = len(rec)
	while pos <= l:
		limb = int(rec[pos])
		pos += 1
		result <<= 7
		result  |= limb & 0x7f
		if limb & 0x80:
			result += 1
		else:
			break
	return (result, pos)


def decode_rec (rec: bytes, bk_no: int) -> object:
	"""
	Decode record.
	(see bitoin-core, chain.h, class CDiskBlockIndex)

	"""
	LdbRec.v, i = __get_varint(rec)			# version
	LdbRec.h, i = __get_varint(rec, i)		# height
	if (LdbRec.h != bk_no):
		eprint("Bk # %d: wrong height (%d)" % (bk_no, LdbRec.h))
	LdbRec.s, i = __get_varint(rec, i)		# status
	LdbRec.t, i = __get_varint(rec, i)		# nTx
	LdbRec.f = LdbRec.d = LdbRec.u = None
	if (LdbRec.s & (BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO)):
		LdbRec.f, i = __get_varint(rec, i)	# File no
		if (LdbRec.f > 2100):
			eprint("Bk # %d: file no is too big: %d" % (bk_no, LdbRec.f))
	if (LdbRec.s & BLOCK_HAVE_DATA):
		LdbRec.d, i = __get_varint(rec, i)	# blk*.dat offset
		if (LdbRec.d > TOOBIGOFFSET):
			eprint("Bk # %d: data offset is too big: %d" % (bk_no, LdbRec.d))
		elif (LdbRec.d < 8):
			eprint("Bk # %d: data offset cannot be < 8: %d" % (bk_no, LdbRec.d))
	if (LdbRec.s & BLOCK_HAVE_UNDO):
		LdbRec.u, i = __get_varint(rec, i)	# rev*.dat offset
		if (LdbRec.u > TOOBIGOFFSET):
			eprint("Bk # %d: undo offset is too big: %d" % (bk_no, LdbRec.u))
		elif (LdbRec.u < 8):
			eprint("Bk # %d: undo offset cannot be < 8: %d" % (bk_no, LdbRec.u))
	if (i != len(rec)):
		eprint("Bk # %d: bytes decoded (%d) != record len (%d)." % (bk_no, i, len(rec)))
	return LdbRec


def get_ldbrec (hash_b: bytes) -> bytes:
	""" Get LevelDB's record of bk. Excluding bk head (last 80 bytes) """
	k = b'\x62' + hash_b
	v = db.Get(k)		# bytearray
	return bytes(v[:-80])


def dump_rec (height: int, rec: bytes):
	""" Record to file. """
	with open("ldb/%06d.ldb" % height, "wb") as f:
		f.write(rec)
		f.close()


def print_rec (ldbrec):
	print("%d\t%d\t%x\t%d" %
		(ldbrec.v, ldbrec.h, ldbrec.s, ldbrec.t), end='\t')
	if (ldbrec.f is None):
		print("-----", end='\t')
	else:
		print("%05d" % ldbrec.f, end='\t')
	if (ldbrec.d):
		print(ldbrec.d, end='\t')
	else:
		print("-----", end='\t')
	if (ldbrec.u):
		print(ldbrec.u)
	else:
		print("-----")

def main(sfn: str, dfn: str):
	global db
	# 0. prepare
	db = leveldb.LevelDB(dbpath)
	bk_no = 0
	df = open(dfn, "wb")
	if not df:
		eprint("Can't open %s to write" % dfn)
	sf = open(sfn, "rb")
	if not sf:
		eprint("Can't open %s to read" % sfn)
	if TODEBUG:	# header
		print("Ver\tHeight\tStatus\tnTx\tFile\tDoffset\tUoffset")
		print("===\t======\t======\t===\t====\t=======\t=======")
	while (True):		# or 'while (bk_no < 100)'
		hash_b = sf.read(32)[::-1]	# 1. get bk hash
		if not hash_b:
			break
		rec = get_ldbrec(hash_b)	# 2. get ldb rec; FIXME: try... except EOFError:...
		r = decode_rec(rec, bk_no)	# 3. decode it
		df.write(pack("<I", r.f))	# 4. write_db
		df.write(pack("<I", r.d))
		if TODUMP:
			dump_rec(bk_no, rec)
		if TODEBUG:
			print_rec(r)
		bk_no += 1
	sf.close()
	df.close()
	if TODEBUG:	# footer
		print("===\t======\t======\t===\t====\t=======\t=======")
	eprint("%s records ok." % bk_no)

if __name__ == '__main__':
	if (len(sys.argv) != 3):
		print("Usage: %s <hashes_file> <output_file>" % sys.argv[0])
	else:
		main(sys.argv[1], sys.argv[2])
