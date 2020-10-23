# -*-coding:utf-8-*-
import sys
import datetime
import calendar
import pymysql
import pyecharts.options as opts
from pyecharts.charts import Line


def get_start_date():

    today = datetime.date.today()
    # today = datetime.datetime.strptime("20200201", "%Y%m%d")
    oneday = datetime.timedelta(days=1)
    m1 = calendar.MONDAY

    while today.weekday() != m1:
        today -= oneday

    start_date = today - datetime.timedelta(weeks=13)

    # print("*" * 50)
    # print(start_date)
    # print("*" * 50)

    cur_week = today.strftime("%W")
    cur_year = today.strftime("%Y")
    start_week = start_date.strftime("%W")
    start_year = start_date.strftime("%Y")
    start_year_lastday = start_year + "1231"
    # print (start_year_lastday)
    start_year_lastweek = datetime.datetime.strptime(
        start_year_lastday, "%Y%m%d").strftime("%W")
    # print (start_year_lastweek)

    return cur_week, cur_year, start_week, start_year, start_year_lastweek, start_date.strftime('%Y-%m-%d %H:%M:%S')

def generate_x_axis():
    dates = get_start_date()

    # python统计周数要比Mysql里少1
    cur_week = int(dates[0]) + 1
    cur_year = int(dates[1])
    start_week = int(dates[2]) + 1
    start_year = int(dates[3])
    start_year_lastweek = int(dates[4]) + 1

    # sys.stdout.write("Current week is: ")
    # print(cur_week)
    # sys.stdout.write("Current year is: ")
    # print(cur_year)
    # sys.stdout.write("Start week is: ")
    # print(start_week)
    # sys.stdout.write("Start year is: ")
    # print(start_year)
    # sys.stdout.write("Start year has: ")
    # sys.stdout.write(str(start_year_lastweek))
    # print(" weeks")

    x_data = []
    if(cur_year == start_year):
        week_ptr = start_week
        while(week_ptr <= cur_week):
            if week_ptr < 10:
                x_data.append(str(start_year)+"0"+str(week_ptr))
                
            else:
                x_data.append(str(start_year)+str(week_ptr))
            week_ptr = week_ptr+1
        # print(x_data)

    else:
        week_ptr = start_week
        while(week_ptr <= start_year_lastweek):
            x_data.append(str(start_year)+str(week_ptr))
            week_ptr = week_ptr+1
        week_ptr = 1
        while(week_ptr <= cur_week):
            if week_ptr < 10:
                x_data.append(str(cur_year)+"0"+str(week_ptr))

            else:
                x_data.append(str(start_year)+str(week_ptr))
            week_ptr = week_ptr+1
        # print(x_data)
    return x_data

def generate_y_axis(x_data, key):
    y_data = []
    for index in x_data:
        flag = 0
        yearweek = int(index)
        for item in res_vehicle_disney:
            if(item['yearweek(begin_timestamp,1)'] == yearweek):
                sys.stdout.write(str(yearweek) + " --------- ")
                print(item[key])
                y_data.append(item[key])
                flag = 1
        if(flag != 1):
            sys.stdout.write(str(yearweek) + " --------- ")
            print(0)
            y_data.append(0)
            flag = 0
    return y_data


def test_func(sql):
    # 链接
    # conn = pymysql.connect(host='szjjh-webserver.szjjh.autowise.ai',
    #                        user='aw_monitor',
    #                        password='123Qwe',
    #                        database='aw_monitor_system',
    #                        charset='utf8',
    #                        cursorclass=pymysql.cursors.DictCursor)

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
            # sql = "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as disney_vehicles from task_execution  where begin_timestamp>=\"%s\" and park_id in ('shanghai_disney') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % get_start_date(
            # )[-1]
            # print(sql)
            # 执行sql语句，返回sql查询成功的记录数目,我只在表中插入一条记录，查询成功最多所以也就一条记录数
            cursor.execute(sql)
            # 连接完数据库并不会自动提交，所以需要手动 commit 你的改动
            # conn.commit()
            res = cursor.fetchall()
            # print(res)
            cursor.close()

    except pymysql.Error as e:
        print("Mysql Error")
        print(e)

    finally:
        conn.close()
        return res


if __name__ == "__main__":

    sql_vehicle_disney = "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as disney_vehicles from task_execution  where begin_timestamp>=\"%s\" and park_id in ('shanghai_disney') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % get_start_date(
    )[-1]
    res_vehicle_disney = test_func(sql_vehicle_disney)
    # sys.stdout.write("Type of the whole result is: ")
    # 使用stdout可以不换行输出
    # print(type(res_vehicle_disney))
    # sys.stdout.write("Type of the a single result is: ")
    # print(type(res_vehicle_disney[0]))
    sys.stdout.write("The result is: \n")
    print(res_vehicle_disney)
    # print(type(res_vehicle_disney[0]['yearweek(begin_timestamp,1)']))

    x_data = generate_x_axis()
    y_data = generate_y_axis(x_data,'disney_vehicles')

(
    Line()
    .set_global_opts(
        title_opts=opts.TitleOpts(
            title="每周投入车辆（单位：辆）", pos_left="center", pos_top="top"),
        tooltip_opts=opts.TooltipOpts(is_show=False),
        legend_opts=opts.LegendOpts(pos_top="50%", pos_left="right"),
        xaxis_opts=opts.AxisOpts(
            type_="category", axislabel_opts={"rotate": 45}),
        yaxis_opts=opts.AxisOpts(
            name="投入车辆数",
            type_="value",
            axistick_opts=opts.AxisTickOpts(is_show=True),
            splitline_opts=opts.SplitLineOpts(is_show=True),
        ),
    )
    .add_xaxis(xaxis_data=x_data)
    .add_yaxis(
        series_name="迪士尼",
        y_axis=y_data,
        symbol="emptyCircle",
        is_symbol_show=True,
        label_opts=opts.LabelOpts(is_show=False),
    )
    .render("basic_line_chart.html")
)
