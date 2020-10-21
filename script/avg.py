import os
import sys



for line in sys.stdin:
    tokens = line.strip('\n').split()
    try:
        print tokens[0] + "\t" + str(float(tokens[1])/float(tokens[2])/float(tokens[3]))
    except Exception as e:
        print e
        print tokens[0] + "\t" + tokens[1]
