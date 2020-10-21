import os
import sys

for line in sys.stdin:
    tokens = line.strip("\n").split()
    ret = []
    print tokens
    for each in tokens:
        if each == "":
            ret.append("0")
        else:
            ret.append(each)

    print "\t".join(ret)

