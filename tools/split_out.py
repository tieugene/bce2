#!/opt/local/bin/python3
"""
Split bcepy/bce2 by fixed blockno parts.
Input: *.txt.gz or stdin
Output: 000-050.txt.gz etc

Test: 15"/630k,
"""

import sys, gzip

# consts
SPLITBY = 10

def walk(infile):
    no_cur = 0
    no_next = 0
    of = None
    tpl = "b\t0"
    tplen = len(tpl)
    for line in infile:
        if line[0] == 'b':
            if line[:tplen] == tpl:
                if of:
                    of.close()
                no_cur = no_next
                no_next += SPLITBY
                of = gzip.open("%03d-%03d.txt.gz" % (no_cur, no_next), "wt")
                tpl = "b\t{}".format(no_next * 1000)
                tplen = len(tpl)
        of.write(line)

def main():
    if len(sys.argv) != 2:
        print("Usage: {} <infile>".format(sys.argv[0]))
        return
    with gzip.open(sys.argv[1], "rt") as infile:
        walk(infile)

if __name__ == '__main__':
    main()
