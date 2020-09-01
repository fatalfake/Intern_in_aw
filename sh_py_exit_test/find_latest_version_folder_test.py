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

s1 = "20191231"

date_p1 = datetime.datetime.strptime(s1,"%Y%m%d").date()
print date_p1, type(date_p1)

s2 = "20200101"

date_p2 = datetime.datetime.strptime(s2,"%Y%m%d").date()
print date_p2, type(date_p2)

gap = date_p2-date_p1


date_p1_calendar = date_p1.isocalendar()
date_p2_calendar = date_p2.isocalendar()

print date_p1_calendar
print date_p2_calendar


if date_p1_calendar[1] < date_p2_calendar[1] or date_p2_calendar[0] < date_p2_calendar[0]:
    print gap, "Should upload"
else:
    print gap, "No need to upload"