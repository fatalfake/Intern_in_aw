# -*-coding:utf-8-*-


import threading  # 线程
import time


def music():
    print('begin to listen music {}'.format(time.ctime()))
    time.sleep(3)
    print('stop to listen music {}'.format(time.ctime()))


def game():
    print('begin to play game {}'.format(time.ctime()))
    time.sleep(5)
    print('stop to play game {}'.format(time.ctime()))


if __name__ == '__main__':
    t1 = threading.Thread(target=music)  # 创建一个线程对象t1 子线程
    t2 = threading.Thread(target=game)  # 创建一个线程对象t2 子线程

    t1.start()
    t2.start()

    # t1.join() #等待子线程执行完 t1不执行完，谁也不准往下走
    t2.join()

    print('ending.......')  # 主线程
    print(time.ctime())
