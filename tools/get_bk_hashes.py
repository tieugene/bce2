#!/bin/env python3
'''
Tool to get all block hashes in right order.
Usage: ... [<file>]
Output:
- plain text as "height	<hash>" (if no <file> defined)
- binary hashes (if <file> defined)
Speed: ~15..18kbk/sec (30..40" at all)
Size: 32kbk/MB (20MB at all)
'''

import sys
from bitcoinrpc.authproxy import AuthServiceProxy

BCQTY = 630
BULK = 1000

def main(f):
	bk_no: int = 0;
	rpc_connection = AuthServiceProxy("http://login:password@127.0.0.1:8332", timeout=120)
	for k in range(BCQTY):
		commands = [ [ "getblockhash", h] for h in range(bk_no, bk_no + BULK) ]
		block_hashes = rpc_connection.batch_(commands)
		for hash in block_hashes:
			if (f):
				f.write(bytes.fromhex(hash))
			else:
				print("%s" % hash)	# "%d\t%s" % (bk_no, hash)
			bk_no += 1

if __name__ == "__main__":
	out = None
	if (len(sys.argv) == 2):
		out = open(sys.argv[1], "wb")
	main(out)
