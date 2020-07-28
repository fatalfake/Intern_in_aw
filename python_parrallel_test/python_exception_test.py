# -*-coding:utf-8-*-

#自定义异常 需要继承Exception
 
class MyException(Exception):
 
    def __init__(self, *args):
        self.args = args
 
#raise MyException('爆出异常吧哈哈')
 
#常见做法定义异常基类,然后在派生不同类型的异常
 
class loginError(MyException):
    def __init__(self, code = 100, message = '登录异常', args = ('登录异常',)):
        self.args = args
        self.message = message
        self.code = code
class loginoutError(MyException):
    def __init__(self):
        self.args = ('退出异常',)
        self.message = '退出异常'
        self.code = 200
#raise loginError() # 这里突然返现 raise引发的异常将中断程序
#
try:
    raise loginError()
except loginError as e:
    print(e) #输出异常
    print(e.code) #输出错误代码
    print(e.message)#输出错误信息

for i in range(100):
    if i % 2 == 0:
        try:
            print ("i is %d" %i)
            print "This is an even number"
            raise loginoutError()
        except loginoutError as e:
            print e
            print e.code
            print e.message
    else:
        try:
            print ("i is %d" %i)
            print "This is an odd number"
            raise loginError()
        except loginError as e:
            print e
            print e.code
            print e.message
