#!/usr/bin/env python3
"""
Filter diff of addresses after comparing get_addrs.pt and bce2 output.
Skip if single multisig == same pubkey
Lines must be:
-|+<bk>\t<tx>\t<vout>\t<addr_type>\t<addr>
Equals are '-...multisig...<key>' == '+...pubkey...<key>'
"""

import re
import sys


def walk(f):
    data = dict()
    # "^[-+]\d+\t\d+\t\d+\t[a-z]+\t1[a-zA-Z1-9]{31,32}$"
    tpl = re.compile(r"^[-+]\d+\t\d+\t\d+\t\w+\t\w+$")
    for line in f:
        line = line.rstrip("\n")
        m = tpl.match(line)
        if m:
            # print("Match")
            sign = line[0]
            part = line[1:].split("\t")
            if (sign == '-' and part[3] == 'multisig') or (sign == '+' and part[3] == 'pubkey'):
                k = (int(part[0]), int(part[1]), int(part[2]))
                v = part[4]
                if line[0] == '-':  # -was
                    data[k] = v
                else:               # +new
                    was = data.get(k)
                    if was:
                        if v == was:    # addrs are eq
                            del data[k]
                        else:
                            print("-{}\t{}\t{}\tmultisig\t{}".format(k[0], k[1], k[2], v))
                            print(line)
                    else:
                        print(line)
            else:
                print(line)
        else:
            print(line)
    for k, v in data.items():
        print("-{}\t{}\t{}\tmultisig\t{}".format(k[0], k[1], k[2], v))


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
