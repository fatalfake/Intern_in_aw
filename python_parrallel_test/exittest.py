import sys

print "Now I exit"

try:
    sys.exit(-1)
except SystemExit as e:
    print "Now I really exit"