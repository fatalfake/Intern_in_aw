import time

current_time = time.strftime("%a %H:%M", time.localtime())
print current_time

start_time = time.time()
start_time_str = time.strftime("%a %H:%M", time.localtime(start_time))
print start_time_str


if current_time == "Wed 10:41":
    print "BINGO!"