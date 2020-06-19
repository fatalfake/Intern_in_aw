import sys
def replaceSpace(s):
    return s.replace(' ','%20')

ss=replaceSpace(sys.argv[1])
exit(ss)
