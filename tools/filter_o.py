#!/usr/bin/env python3
"""
Filter diff by ^o after comparing bcepy and bce2 output.
Lines must be:
-|+o\t...
"""

import re
import sys

MAXDELTA = 1


def walk(f):
    data = dict()   # 
    tpl = re.compile(r"^[-+]o\t\d+\t\d+\t\d+\t\d+$")
    l1 = l2 = None
    for line in f:
        line = line.rstrip("\n")
        m = tpl.match(line)
        if m:
            part = line.split("\t")
            key = (int(part[1]), int(part[2]))
            val = (int(part[3]), int(part[4]))
            if line[0] == '-':
                data[key] = val
            else:
                was = data.get(key)
                if was:
                    if val[1] == was[1]:
                        del data[key]
                        delta = val[0] - was[0]
                        if abs(delta) > MAXDELTA:
                            print("o\t{}\t{}\t{}\t{}\t{0:+}".format(key[0], key[1], was[1], was[0], delta))
                    else:
                        print("-o\t{}\t{}\t{}\t{}".format(key[0], key[1], was[0], was[1]))
                        print(line)
                else:
                    print(line)
        else:
            print(line)
    for k, v in data.items():
        print("-o\t{}\t{}\t{}\t{}".format(k[0], k[1], v[0], v[1]))


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
