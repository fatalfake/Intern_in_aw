import subprocess
import re


cmd = "netstat -lntp | grep 127.0.0.1"

s = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)

p = s.stdout.read()

print len(p)
print p

k = p.split('\n')
k.remove(k[-1])

forbid_port_list = []

for i in k:
    # print i
    j = re.split(r"[ ]+", i)
    # print j
    l = j[3].split(':')[1]
    # print l
    forbid_port_list.append(l)


forbid_port_list.append('100')
forbid_port_list.append('200')

forbid_port_list.sort()
print int(forbid_port_list[0])

print forbid_port_list

ppp = "631"
# ppp = 100
if ppp in forbid_port_list:
    print "YES"
    while True:
        ppp = str(int(ppp)+1)
        if ppp not in forbid_port_list:
            break

print ppp

if ppp not in forbid_port_list:
    print "NO"

