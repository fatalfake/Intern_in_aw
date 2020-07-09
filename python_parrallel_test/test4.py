# -*-coding:utf-8-*-

#计算密集型任务，多线程不起作用


import time
from threading import Thread


def add():
    sum = 0
    i = 1
    while i <= 1000000:
        sum += i
        i += 1
    print('sum:', sum)


def mul():
    sum2 = 1
    i = 1
    while i <= 100000:
        sum2 = sum2 * i
        i += 1
    print('sum2:', sum2)


start = time.time()
t1 = Thread(target=add)
t2 = Thread(target=mul)

l = []
l.append(t1)
l.append(t2)

for t in l:
   t.start()

for t in l:
    t.join()

print('cost time %s' % (time.time()-start))
