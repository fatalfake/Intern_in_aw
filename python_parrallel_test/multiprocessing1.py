#coding=utf-8

import multiprocessing
import os		# 获取pid用
import time		# 延时用

# 子进程要执行的函数
def child_proc(name):
    print("child process "+ name + " pid: " + str(os.getpid()))
    # print(f'child process {name} pid: {os.getpid()}') 此为python3.6之后写法
    time.sleep(3)
    print(name + " finish")
    # print(f'{name} finish')

# 主进程，必须在主模块中执行
if __name__ == '__main__':
    print("parent process "+ str(os.getpid()) + " is running")
	# print(f'parent process {os.getpid()} is running')

	# 生成子进程
    p1 = multiprocessing.Process(target = child_proc, args = ('child-1',))
    p2 = multiprocessing.Process(target = child_proc, args = ('child-2',))
    p1.start()
    p2.start()

    print("parent process " + str(os.getpid()) + " is end")
	# print(f'parent process {os.getpid()} is end')# print(f'child process {name} pid: {os.getpid()}') 此为python3.6之后写法

    ns = "mynamespace"
    rosevaluation_cmd = "roslaunch aw_evaluation start_evaluation.launch __ns:=" + ns
    print(rosevaluation_cmd)