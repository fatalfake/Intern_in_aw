# -*-coding:utf-8-*-

import threading  # 线程
import time


def music():
    print('begin listening to music {}').format(time.ctime())
    time.sleep(3)
    print('stop listening to music {}').format(time.ctime())


def game():
    print('begin playing game {}').format(time.ctime())
    time.sleep(5)
    print('stop playing game {}').format(time.ctime())


if __name__ == '__main__':
    music()
    game()
    print('ending......')
