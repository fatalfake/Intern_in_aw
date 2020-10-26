# -*-coding:utf-8-*-
import sys
import datetime
import calendar
import pymysql
import pyecharts.options as opts
from pyecharts.charts import Line
from pyecharts.charts import Page
from pyecharts.globals import ThemeType


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


def generate_y_axis(x_data, res, key):
    y_data = []
    print("YEARWEEK ------- %s" %key)
    for index in x_data:
        flag = 0
        yearweek = int(index)
        for item in res:
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
            # sql = "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as disney_vehicles from task_execution  where begin_timestamp>=\"%s\" and park_id in ('shanghai_disney') group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % get_start_date(
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


def draw_test(x_data, y_data, chart_title, yaxis):
    line = (
        Line(
            init_opts=opts.InitOpts(
                page_title="Week-Vehicle", bg_color=None, theme=ThemeType.CHALK),
        )

        .set_global_opts(
            # title_opts=opts.TitleOpts(
            #     title="每周投入车辆（单位：辆）", pos_left="center", pos_top="top"),
            title_opts=opts.TitleOpts(
                title=chart_title),
            tooltip_opts=opts.TooltipOpts(is_show=True, trigger="axis"),
            legend_opts=opts.LegendOpts(
                type_="scroll", pos_top="33%", pos_left="right", orient="vertical", pos_right="0px"),
            xaxis_opts=opts.AxisOpts(
                name="年-周", name_location="end", name_gap=2, type_="category", axislabel_opts={"rotate": 45}),
            yaxis_opts=opts.AxisOpts(
                name=yaxis,
                name_location="end",
                name_gap=20,
                type_="value",
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
            ),
        )
        .add_xaxis(xaxis_data=x_data)
        .add_yaxis(
            series_name="北京",
            y_axis=y_data[0],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="迪士尼",
            y_axis=y_data[1],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="汽车博览园",
            y_axis=y_data[2],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="北创新港",
            y_axis=y_data[3],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="波雄工厂",
            y_axis=y_data[4],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="沃尔玛",
            y_axis=y_data[5],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="广州",
            y_axis=y_data[6],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="安亭开放道路",
            y_axis=y_data[7],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="松江",
            y_axis=y_data[8],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        .add_yaxis(
            series_name="苏州相城",
            y_axis=y_data[9],
            is_symbol_show=False,
            label_opts=opts.LabelOpts(is_show=False),
        )
        # .render(filename)
    )
    return line


def sql_vehicle_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as %s_vehicles from task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)

def sql_duration_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1), sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as %s_duration  from task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)

def sql_duration_avg_generate(str1, str2, str3):
    return "select week(begin_timestamp,1),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as %s_duration, count(distinct(vehicle_id)) ,count(distinct(date_format(begin_timestamp,'%%Y%%m%%d'))) from  task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by year(begin_timestamp),week(begin_timestamp,1);" %(str1, str2, str3)

def sql_distance_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1), sum(distance)/1000 as %s_distance from task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)

if __name__ == "__main__":

    site_list = ["beijing", "disney", "auto", "innovation", "boschung",
                 "walmart", "guangzhou", "anting", "songjiang", "suzhou"]
    park_list = ["'beijing_huanbao_park','beijing_haidian_jiaxiao'",
                 "'shanghai_disney'",
                 "'shanghai_auto_expo'",
                 "'shanghai_innovation_port'",
                 "'boschung_factory'",
                 "'pheonix_walmart'",
                 "'guangzhou_longhu','guangzhou_longhu_all','guangzhou_longhu_park'",
                 "'shanghai_anting_newtown'",
                 "'shanghai_songjiang_test','shanghai_songjiang_xinfeng','shanghai_tuspark'",
                 "'suzhou_station_square','suzhou_xiangcheng_all','suzhou_xiangcheng_demo','suzhou_demo_park'"]

    start_time = get_start_date()[-1]

    x_data = generate_x_axis()
    x_data_temp = []
    for item in x_data:
        item1 = list(item)
        item1.insert(4, '年')
        item1.insert(5, '第')
        item1.append('周')
        del(item1[0])
        del(item1[0])
        x_data_temp.append("".join(item1))

    sql_vehicle_list = []
    sql_duration_list = []
    sql_distance_list = []

    res_vehicle_list = []
    res_duration_list = []
    res_distance_list = []

    y_data_vehicle = []
    y_data_duration = []
    y_data_distance = []

    for i in range(len(site_list)):
        sql_vehicle_list.append(sql_vehicle_generate(
            site_list[i], start_time, park_list[i]))
        sql_duration_list.append(sql_duration_generate(
            site_list[i], start_time, park_list[i]))
        sql_distance_list.append(sql_distance_generate(
            site_list[i], start_time, park_list[i]))

        res_vehicle_list.append(test_func(sql_vehicle_list[i]))
        res_duration_list.append(test_func(sql_duration_list[i]))
        res_distance_list.append(test_func(sql_distance_list[i]))

        y_data_vehicle.append(generate_y_axis(
            x_data, res_vehicle_list[i], "%s_vehicles" %site_list[i])) 
        y_data_duration.append(generate_y_axis(
            x_data, res_duration_list[i], "%s_duration" %site_list[i]))
        y_data_distance.append(generate_y_axis(
            x_data, res_distance_list[i], "%s_distance" %site_list[i]))
    
    # sys.stdout.write("Type of the whole result is: ")
    # 使用stdout可以不换行输出
    # print(type(res_vehicle_disney))
    # sys.stdout.write("Type of the a single result is: ")
    # print(type(res_vehicle_disney[0]))
    # sys.stdout.write("The result is: \n")
    # print(res_vehicle_disney)
    # print(type(res_vehicle_disney[0]['yearweek(begin_timestamp,1)']))

    # print(x_data)
    # print(x_data_temp)


    page = Page()
    line1 = draw_test(x_data_temp, y_data_vehicle, "每周投入车辆（单位：辆）", "投入车辆数")
    page.add(line1)


    line2 = draw_test(x_data_temp, y_data_duration, "每周总测试时间（单位：小时）", "测试小时数")
    page.add(line2)

    line3 = draw_test(x_data_temp, y_data_distance, "每周总测试里程（单位：公里）", "公里")
    page.add(line3)
    page.render("111.html")
