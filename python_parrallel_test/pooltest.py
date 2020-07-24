#coding=utf-8

import multiprocessing
import datetime
import time


def func(msg):
    print("msg:", msg)
    time.sleep(3)
    print("end,", msg)

if __name__ == "__main__":

    start_t = datetime.datetime.now()
   
    # 这里设置允许同时运行的的进程数量要考虑机器cpu的数量，进程的数量最好小于cpu的数量，
    # 因为即使大于cpu的数量，增加了任务调度的时间，效率反而不能有效提高

    num_cores = int(multiprocessing.cpu_count())
    print("本地计算机有: " + str(num_cores) + " 核心")
    pool = multiprocessing.Pool(num_cores)
    print ("This is main process")
    item_list = ['processes1' ,'processes2' ,'processes3' ,'processes4' ,'processes5' , \
                        'processes6' ,'processes7' ,'processes8' ,'processes9' ,'processes10' , \
                        'processes11' ,'processes12' ,'processes13' ,'processes14' ,'processes15' , \
                        'processes16' ,'processes17' ,'processes18' ,'processes19' ,'processes20' ,]
    count = len(item_list)
    for item in item_list:
        msg = "hello %s" %item
        # 维持执行的进程总数为processes，当一个进程执行完毕后会添加新的进程进去
        pool.apply_async(func, (msg,))

    print("Wait for subprocesses")
    pool.close()
    pool.join()  # 调用join之前，先调用close函数，否则会出错。执行完close后不会有新的进程加入到pool,join函数等待所有子进程结束
    print("All subprocesses ended")

    end_t = datetime.datetime.now()
    elapsed_sec = (end_t - start_t).total_seconds()
    print("多进程计算 共消耗: " + "{:.2f}".format(elapsed_sec) + " 秒")

# import math
# import datetime
# import multiprocessing as mp


# def train_on_parameter(name, param):
#     result = 0
#     for num in param:
#         result += math.sqrt(num * math.tanh(num) / math.log2(num) / math.log10(num))
#     return {name: result}


# if __name__ == '__main__':

#     start_t = datetime.datetime.now()

#     num_cores = int(mp.cpu_count())
#     print("本地计算机有: " + str(num_cores) + " 核心")
#     pool = mp.Pool(num_cores)
#     param_dict = {'task1': list(range(10, 30000000)),
#                   'task2': list(range(30000000, 60000000)),
#                   'task3': list(range(60000000, 90000000)),
#                   'task4': list(range(90000000, 120000000)),
#                   'task5': list(range(120000000, 150000000)),
#                   'task6': list(range(150000000, 180000000)),
#                   'task7': list(range(180000000, 210000000)),
#                   'task8': list(range(210000000, 240000000))}
#     results = [pool.apply_async(train_on_parameter, args=(name, param)) for name, param in param_dict.items()]
#     results = [p.get() for p in results]

#     end_t = datetime.datetime.now()
#     elapsed_sec = (end_t - start_t).total_seconds()
#     print("多进程计算 共消耗: " + "{:.2f}".format(elapsed_sec) + " 秒")