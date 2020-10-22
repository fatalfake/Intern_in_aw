# -*-coding:utf-8-*-
import pymysql
import datetime
import calendar


def get_start_date():

    today = datetime.date.today()
    oneday = datetime.timedelta(days=1)
    m1 = calendar.MONDAY

    while today.weekday() != m1:
        today -= oneday

    start_date = today - datetime.timedelta(weeks=14)

    return start_date.strftime('%Y-%m-%d %H:%M:%S')


# 链接
conn = pymysql.connect(host='szjjh-webserver.szjjh.autowise.ai',
                       user='aw_monitor',
                       password='123Qwe',
                       database='aw_monitor_system',
                       charset='utf8',
                       cursorclass=pymysql.cursors.DictCursor)

# 执行完毕返回的结果集默认以元组显示
# cursorclass=pymysql.cursors.DictCursor 配置结果集为字典形式

try:
    # 游标
    with conn.cursor() as cursor:
        # 执行sql语句
        sql = "select year(begin_timestamp),  week(begin_timestamp,1),  count(distinct(vehicle_id)) as beijing_vehicles from task_execution  where begin_timestamp > \"%s\" and park_id in ('beijing_huanbao_park','beijing_haidian_jiaxiao') group by year(begin_timestamp),week(begin_timestamp,1) order by year(begin_timestamp), week(begin_timestamp, 1)" % get_start_date()
        print(sql)
        # 执行sql语句，返回sql查询成功的记录数目,我只在表中插入一条记录，查询成功最多所以也就一条记录数
        cursor.execute(sql)
        # 连接完数据库并不会自动提交，所以需要手动 commit 你的改动
        # conn.commit()
        res = cursor.fetchall()
        print(res)
        cursor.close()

except pymysql.Error as e:
    print ("Mysql Error")
    print (e)

finally:
    conn.close()
