# -*- coding:utf-8 -*-

import datetime
import subprocess

cmd = "cd /home/autowise/桌面/newdir/test; read -r -a arr <<< `ls`; echo ${arr[-1]}"

p = subprocess.Popen(cmd, executable='/bin/bash', stdout=subprocess.PIPE, shell=True)

s = p.stdout.read()

s = s.strip()

print s, type(s)

date_p = datetime.datetime.strptime(s,"%Y%m%d").date()
print date_p, type(date_p)

s1 = "20200908"

date_p1 = datetime.datetime.strptime(s1,"%Y%m%d").date()
print date_p1, type(date_p1)

s2 = "20200911"

date_p2 = datetime.datetime.strptime(s2,"%Y%m%d").date()
print date_p2, type(date_p2)

gap = date_p2-date_p1

if gap.days >= 3:
    print gap, "More than 3 days"
else:
    print gap