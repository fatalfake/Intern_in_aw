# -*- coding: utf-8 -*-
import datetime

# help(datetime)

d1 = datetime.date(2020, 8, 31)
d2 = datetime.date(2020, 11, 1)
print d2-d1

print d1.isocalendar() #查看d1是哪一年的第几天和第几周。注意：有时候年末最后几天会算到第二年的第一周，如下所示：

d3 = datetime.date(2019, 12, 31)

print d3.isocalendar() #2019-12-31被算作了2020的第一周的周二

print d1.timetuple() #格式为：(d.year, d.month, d.day, d.hour, d.minute, d.second, d.weekday()(周一为0), 当年的第几天(1月1日为0), isdst(夏令时标识符,实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。))

d=datetime.timedelta(weeks=20)

print d1+d #查看d1之后20周是什么日期

print d1+datetime.timedelta(days=1)
print d1-datetime.timedelta(days=3)

print datetime.timedelta(weeks=40, days=84, hours=23, minutes=50, seconds=600) #40周*7 = 280天， 280+84 = 364， 23小时+50分钟+600秒=最后的一天