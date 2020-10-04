#!/opt/local/bin/python3
"""
Get block info from LevelDB.
Input: LevelDB path
Output: bk file+offset (8 bytes per bk)

Test: 15"/630k,

Err: 645179 duped
"""

import os, sys
import argparse
from struct import pack
from collections import namedtuple
import plyvel

# consts
BLOCK_HAVE_DATA = 8
BLOCK_HAVE_UNDO = 16
TOOBIGOFFSET = 256 << 20  # 256MB > any blk*.dat
MAXFILENO = 2999
header = "Height\tStat\tnTx  \tFile\tDoffset"
strike = "======\t====\t=====\t====\t========"

# vars
LdbRec = namedtuple('LdbRec', ['v', 'h', 's', 't', 'f', 'd', 'u'])


def eprint(s: str):
    print(s, file=sys.stderr)


def __print_rec(ldbrec):
    print("%6d\t%4x\t%5d\t%04d\t%08x" %
          (ldbrec.h, ldbrec.s, ldbrec.t, ldbrec.f, ldbrec.d))


def bytes2hex(b: bytes) -> str:
    return b.hex()


def _get_varint(rec: bytes, pos: int = 0) -> tuple:
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
        result |= limb & 0x7f
        if limb & 0x80:
            result += 1
        else:
            break
    return (result, pos)


def decode_rec(rec: bytes, bk_qty: int) -> object:
    """
    Decode record.
    (see bitoin-core, chain.h, class CDiskBlockIndex)
    """
    LdbRec.v, i = _get_varint(rec)  # 1. version
    LdbRec.h, i = _get_varint(rec, i)  # 2. height
    if (LdbRec.h >= bk_qty):
        return None
    LdbRec.s, i = _get_varint(rec, i)  # 3. status
    LdbRec.t, i = _get_varint(rec, i)  # 4. nTx
    LdbRec.f = LdbRec.d = LdbRec.u = None
    if (LdbRec.s & (BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO)):
        LdbRec.f, i = _get_varint(rec, i)  # 5. File no
        if (LdbRec.f > MAXFILENO):
            eprint("Bk # %d: file no is too big: %d" % (LdbRec.h, LdbRec.f))
    if (LdbRec.s & BLOCK_HAVE_DATA):
        LdbRec.d, i = _get_varint(rec, i)  # 6. blk*.dat offset
        if (LdbRec.d > TOOBIGOFFSET):
            eprint("Bk # %d: data offset is too big: %d" % (bk_no, LdbRec.d))
        elif (LdbRec.d < 8):
            eprint("Bk # %d: data offset cannot be < 8: %d" % (bk_no, LdbRec.d))
    if (LdbRec.s & BLOCK_HAVE_UNDO):
        LdbRec.u, i = _get_varint(rec, i)  # 7. rev*.dat offset
        if (LdbRec.u > TOOBIGOFFSET):
            eprint("Bk # %d: undo offset is too big: %d" % (bk_no, LdbRec.u))
        elif (LdbRec.u < 8):
            eprint("Bk # %d: undo offset cannot be < 8: %d" % (bk_no, LdbRec.u))
    if (i != len(rec)):
        eprint("Bk # %d: bytes decoded (%d) != record len (%d)." % (bk_no, i, len(rec)))
    return LdbRec


def walk(ldn: str, ofn: str, qty: int, verbose: bool):
    """
    @param ldn: LevelDB directory
    @param ofn: output file name (2x4 bytes per block)
    @param qty: Blocks process up to
    @param verbose: no comments
    @return: None
    """
    # global LdbRec
    # 0. prepare
    if verbose:  # header
        print(header)
        print(strike)
    # 1. load
    bk_count = bk_found = 0
    bk_dict = dict()
    db = plyvel.DB(ldn, create_if_missing=False)
    for rec in db.iterator(include_key=False, prefix=b'\x62'):  # ^'b'
        rec = decode_rec(rec[:-80], qty)
        bk_count += 1
        if not rec:
            continue
        r = bk_dict.get(rec.h, None)
        if (r):
            if (r[0] == rec.f and r[1] == rec.d):
                continue
            else:
                eprint("Bk {} was already found. File/offset: was {}.{}, new {}.{}".format(rec.h, r[0], r[1], rec.f, rec.d))
                break;
        bk_dict[rec.h] = (rec.f, rec.d)
        bk_found += 1
        if verbose:
            __print_rec(rec)
        if bk_found >= qty:
            break
    db.close()
    if (verbose):
        print(strike)
    if bk_found != qty:
        eprint("Blocks found {} from required {}".format(bk_found, qty))
        return
    eprint("{}/{} blocks loaded.".format(bk_found, bk_count))
    # 2. save
    with open(ofn, "wb") as of:
        for i in range(qty):
            f, o = bk_dict[i]
            of.write(pack("<I", f))
            of.write(pack("<I", o))


def init_cli():
    """ Handle CLI """
    parser = argparse.ArgumentParser(description='File-offset extractor.')
    parser.add_argument('-l', '--ldb', metavar='<dir>', type=str, nargs=1,
                        help='LevelDB dir')
    parser.add_argument('-o', '--outfile', metavar='<file>', type=str, nargs=1,
                        help='Output file name')
    parser.add_argument('-n', '--num', metavar='n', type=int, nargs='?', default=1,
                        help='Blocks to process (default=1)')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Debug (default=false)')
    return parser


def main():
    """
    CLI commands/options handler.
    """
    parser = init_cli()
    args = parser.parse_args()
    if (not args.ldb) or (not args.outfile):
        parser.print_help()
        return
    ldir = args.ldb[0]
    if not os.path.isdir(ldir):
        eprint("LevelDB '{}' not exist or is not dir.".format(ldir))
        return
    ofile = args.outfile[0]
    return walk(ldir, ofile, args.num, args.verbose)


if __name__ == '__main__':
    main()
