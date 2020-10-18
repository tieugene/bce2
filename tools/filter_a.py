#!/usr/bin/env python3
"""
Filter diff by ^a after comparing bcepy and bce2 output.
Lines must be:
-|+a\t...
"""

import re
import sys
import json

MAXDELTA = 1

def walk(f):
    data = dict()   # id: (str/list, orig_str, qty)
    tpl = re.compile(r"^[-+]a\t\d+\t")  # id | str | qty
    l1 = l2 = None
    for line in f:
        line = line.rstrip("\n")
        m = tpl.match(line)
        if m:
            part = line.split("\t")
            key = int(part[1])
            repr = json.loads(part[2])
            if (type(repr) == list):
                repr.sort()
            val = (repr, part[2], int(part[3]))
            if line[0] == '-':  # -was
                data[key] = val
            else:               # +new
                was = data.get(key)
                if was:
                    del data[key]
                    if val[0] != was[0]:    # eq
                        print("-a\t{}\t{}\t{}".format(key, was[1], was[2]))
                        print(line)
                else:
                    print(line)
        else:
            print(line)
    for k, v in data.items():
        print("-a\t{}\t{}\t{}".format(k, v[1], v[2]))

def main():
    if len(sys.argv) > 1:
        f = open(sys.argv[1], "rt")
    else:
        f = sys.stdin
    if f:
        walk(f)
    else:
        print("Usage: {} [<infile>]".format(sys.argv[0]))


if __name__ == '__main__':
    main()
