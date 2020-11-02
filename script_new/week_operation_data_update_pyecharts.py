# -*-coding:utf-8-*-
import sys
import datetime
import calendar
import pymysql
import pyecharts.options as opts
from pyecharts.charts import Line
from pyecharts.charts import Bar
from pyecharts.charts import Page
from pyecharts.globals import ThemeType
from pyecharts.globals import WarningType
from pyecharts.render import make_snapshot
from snapshot_selenium import snapshot
# from snapshot_phantomjs import snapshot
WarningType.ShowWarning = False


def get_start_date():

    today = datetime.date.today()
    oneday = datetime.timedelta(days=1)
    m1 = calendar.MONDAY

    while today.weekday() != m1:
        today -= oneday

    start_date = today - datetime.timedelta(weeks=13)

    cur_week = today.strftime("%W")
    cur_year = today.strftime("%Y")
    start_week = start_date.strftime("%W")
    start_year = start_date.strftime("%Y")
    start_year_lastday = start_year + "1231"
    start_year_lastweek = datetime.datetime.strptime(
        start_year_lastday, "%Y%m%d").strftime("%W")

    return cur_week, cur_year, start_week, start_year, start_year_lastweek, start_date.strftime('%Y-%m-%d %H:%M:%S')


def generate_x_axis():
    dates = get_start_date()

    # python统计周数要比Mysql里少1
    cur_week = int(dates[0]) + 1
    cur_year = int(dates[1])
    start_week = int(dates[2]) + 1
    start_year = int(dates[3])
    start_year_lastweek = int(dates[4]) + 1

    x_data = []
    if(cur_year == start_year):
        week_ptr = start_week
        while(week_ptr <= cur_week):
            if week_ptr < 10:
                x_data.append(str(start_year)+"0"+str(week_ptr))

            else:
                x_data.append(str(start_year)+str(week_ptr))
            week_ptr = week_ptr+1

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
    return x_data


def generate_y_axis(x_data, res, key):
    y_data = []
    print("YEARWEEK ------- %s" % key)
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


def generate_y_axis_avg(x_data, res, key1, key2, key3):
    y_data = []
    print("YEARWEEK ------- %s --- %s --- %s" % (key1, key2, key3))
    for index in x_data:
        flag = 0
        yearweek = int(index)
        for item in res:
            if(item['yearweek(begin_timestamp,1)'] == yearweek):
                sys.stdout.write(str(yearweek) + " --------- ")
                sys.stdout.write(str(item[key1]) + " --------- ")
                sys.stdout.write(str(item[key2]) + " --------- ")
                print(item[key3])
                avg = (float(item[key1]) / float(item[key2])
                       ) / float(item[key3])
                y_data.append(avg)
                flag = 1
        if(flag != 1):
            sys.stdout.write(str(yearweek) + " --------- ")
            print(0)
            y_data.append(0)
            flag = 0
    return y_data


def generate_y_axis_static(x_data, res, key1, key2, key3, key4, key5, key6):
    y_data_list = []
    y_data_num_takeover = []
    y_data_num_stop = []
    y_data_duration = []
    y_data_mile_dist = []
    y_data_num_v_id = []
    y_data_duration_avg = []
    print("YEARWEEK ------- %s --- %s --- %s --- %s --- %s --- %s" %
          (key1, key2, key3, key4, key5, key6))
    for index in x_data:
        flag = 0
        yearweek = int(index)
        for item in res:
            if(item['yearweek(begin_timestamp,1)'] == yearweek):
                sys.stdout.write(str(yearweek) + " --------- ")
                sys.stdout.write(str(item[key1]) + " --------- ")
                sys.stdout.write(str(item[key2]) + " --------- ")
                sys.stdout.write(str(item[key3]) + " --------- ")
                sys.stdout.write(str(item[key4]) + " --------- ")
                sys.stdout.write(str(item[key5]) + " --------- ")
                y_data_num_takeover.append(item[key1])
                y_data_num_stop.append(item[key2])
                y_data_duration.append(item[key3])
                y_data_mile_dist.append(item[key4])
                y_data_num_v_id.append(item[key5])
                avg = (float(item[key3]) / float(item[key5])) / 7
                print(avg)
                y_data_duration_avg.append(avg)
                flag = 1
        if(flag != 1):
            sys.stdout.write(str(yearweek) + " --------- ")
            print(0)
            y_data_num_takeover.append(0)
            y_data_num_stop.append(0)
            y_data_duration.append(0)
            y_data_mile_dist.append(0)
            y_data_num_v_id.append(0)
            y_data_duration_avg.append(0)
            flag = 0

    y_data_list.append(y_data_num_takeover)
    y_data_list.append(y_data_num_stop)
    y_data_list.append(y_data_duration)
    y_data_list.append(y_data_mile_dist)
    y_data_list.append(y_data_num_v_id)
    y_data_list.append(y_data_duration_avg)
    return y_data_list


def execute_sql(conn, sql):

    try:
        with conn.cursor() as cursor:
            cursor.execute(sql)
            res = cursor.fetchall()
            cursor.close()

    except pymysql.Error as e:
        print("Mysql Error")
        print(e)

    finally:
        return res


def draw_chart1(x_data, y_data, chart_title, yaxis):
    line = (
        Line(init_opts=opts.InitOpts(bg_color="#FFF",)
             )

        .set_global_opts(
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
            label_opts=opts.LabelOpts(is_show=False),
            color="#982f2a",
        )
        .add_yaxis(
            series_name="迪士尼",
            y_axis=y_data[1],
            label_opts=opts.LabelOpts(is_show=False),
            color="#2f4554",
        )
        .add_yaxis(
            series_name="汽车博览园",
            y_axis=y_data[2],
            label_opts=opts.LabelOpts(is_show=False),
            color="#86df27",
        )
        .add_yaxis(
            series_name="创新港",
            y_axis=y_data[3],
            label_opts=opts.LabelOpts(is_show=False),
            color="#ff420e",
        )
        .add_yaxis(
            series_name="波雄工厂",
            y_axis=y_data[4],
            label_opts=opts.LabelOpts(is_show=False),
            color="#f3f114",
        )
        .add_yaxis(
            series_name="沃尔玛",
            y_axis=y_data[5],
            label_opts=opts.LabelOpts(is_show=False),
            color="#007f1f",
        )
        .add_yaxis(
            series_name="广州",
            y_axis=y_data[6],
            label_opts=opts.LabelOpts(is_show=False),
            color="#dca656"
        )
        .add_yaxis(
            series_name="安亭开放道路",
            y_axis=y_data[7],
            label_opts=opts.LabelOpts(is_show=False),
            color="#b875c6",
        )
        .add_yaxis(
            series_name="松江",
            y_axis=y_data[8],
            label_opts=opts.LabelOpts(is_show=False),
            color="#050000",
        )
        .add_yaxis(
            series_name="苏州相城",
            y_axis=y_data[9],
            label_opts=opts.LabelOpts(is_show=False),
            color="#08eeff",
        )
        .set_series_opts(
            linestyle_opts=opts.LineStyleOpts(
                width=2,
            )
        )
    )
    return line


def draw_chart2(x_data, y_data1, y_data2, yaxis1, yaxis2, unit1, unit2, title1):
    line = (
        Line(init_opts=opts.InitOpts(bg_color="#FFF",)
             )
        .add_xaxis(x_data)
        .add_yaxis(yaxis1, y_data1, label_opts=opts.LabelOpts(is_show=False),)
        .set_global_opts(
            yaxis_opts=opts.AxisOpts(
                axistick_opts=opts.AxisTickOpts(is_show=True),
                splitline_opts=opts.SplitLineOpts(is_show=True),
                axislabel_opts=opts.LabelOpts(formatter="{value} %s" % unit1)
            ),
            title_opts=opts.TitleOpts(title=title1),
            tooltip_opts=opts.TooltipOpts(is_show=True, trigger="axis"),
        )
        .extend_axis(
            yaxis=opts.AxisOpts(
                axislabel_opts=opts.LabelOpts(formatter="{value} %s" % unit2),
            )
        )
        .set_series_opts(linestyle_opts=opts.LineStyleOpts(width=2,),)
    )
    line1 = Line().add_xaxis(x_data).add_yaxis(
        yaxis2, y_data2, yaxis_index=1, label_opts=opts.LabelOpts(is_show=False),).set_series_opts(linestyle_opts=opts.LineStyleOpts(width=2,),)
    line.overlap(line1)
    return line


def sql_vehicle_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as %s_vehicles from task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)


def sql_duration_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1), sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as %s_duration  from task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)


def sql_duration_avg_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1), sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as %s_duration, count(distinct(vehicle_id)) as num_v_id ,count(distinct(date_format(begin_timestamp,'%%Y%%m%%d'))) as days from  task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)


def sql_distance_generate(str1, str2, str3):
    return "select yearweek(begin_timestamp,1), sum(distance)/1000 as %s_distance from task_execution  where begin_timestamp>\"%s\" and park_id in (%s) group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1);" % (str1, str2, str3)


def sql_static_generate(str1):
    return "select yearweek(begin_timestamp,1), sum(takeover_times) as num_takeover, sum(stop_times) as num_stop,sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as duration, sum(distance)/1000 as mile_dist, count(distinct(vehicle_id)) as num_v_id from task_execution where begin_timestamp > \"%s\" group by yearweek(begin_timestamp,1) order by yearweek(begin_timestamp,1); " % (str1)


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
    sql_duration_avg_list = []

    res_vehicle_list = []
    res_duration_list = []
    res_distance_list = []
    res_duration_avg_list = []

    y_data_vehicle = []
    y_data_duration = []
    y_data_distance = []
    y_data_duration_avg = []

    conn = pymysql.connect(host='szjjh-webserver.szjjh.autowise.ai',
                           user='aw_monitor',
                           password='123Qwe',
                           database='aw_monitor_system',
                           charset='utf8',
                           cursorclass=pymysql.cursors.DictCursor)

    for i in range(len(site_list)):
        sql_vehicle_list.append(sql_vehicle_generate(
            site_list[i], start_time, park_list[i]))
        sql_duration_list.append(sql_duration_generate(
            site_list[i], start_time, park_list[i]))
        sql_distance_list.append(sql_distance_generate(
            site_list[i], start_time, park_list[i]))
        sql_duration_avg_list.append(sql_duration_avg_generate(
            site_list[i], start_time, park_list[i]))

        res_vehicle_list.append(execute_sql(conn, sql_vehicle_list[i]))
        res_duration_list.append(execute_sql(conn, sql_duration_list[i]))
        res_distance_list.append(execute_sql(conn, sql_distance_list[i]))
        res_duration_avg_list.append(execute_sql(conn, sql_duration_avg_list[i]))

        y_data_vehicle.append(generate_y_axis(
            x_data, res_vehicle_list[i], "%s_vehicles" % site_list[i]))
        y_data_duration.append(generate_y_axis(
            x_data, res_duration_list[i], "%s_duration" % site_list[i]))
        y_data_distance.append(generate_y_axis(
            x_data, res_distance_list[i], "%s_distance" % site_list[i]))
        y_data_duration_avg.append(generate_y_axis_avg(
            x_data, res_duration_avg_list[i], "%s_duration" % site_list[i], "num_v_id", "days"))

    sql_static = sql_static_generate(start_time)
    res_static = execute_sql(conn, sql_static)
    y_data_static = generate_y_axis_static(
        x_data, res_static, "num_takeover", "num_stop", "duration", "mile_dist", "num_v_id", "duration_avg")

    conn.close()

    page = Page()
    line1 = draw_chart1(x_data_temp, y_data_vehicle, "每周投入车辆（单位：辆）", "投入车辆数")
    page.add(line1)
    make_snapshot(snapshot, line1.render(), "每周投入车辆.png")

    line2 = draw_chart1(x_data_temp, y_data_duration,
                        "每周总测试时间（单位：小时）", "测试小时数")
    page.add(line2)
    make_snapshot(snapshot, line2.render(), "每周总测试时间.png")

    line3 = draw_chart1(x_data_temp, y_data_distance, "每周总测试里程（单位：公里）", "公里")
    page.add(line3)
    make_snapshot(snapshot, line3.render(), "每周总测试里程.png")

    line4 = draw_chart1(x_data_temp, y_data_duration_avg,
                        "每周平均每台车每天测试时长（单位：小时）", "每周平均每台车每天测试时长")
    page.add(line4)
    make_snapshot(snapshot, line4.render(), "每周平均每台车每天测试时长.png")

    y_axis = ["接管次数", "停止次数", "测试时间", "测试里程", "投入车辆数", "平均每台车每天测试时长"]
    unit = ["次", "次", "小时", "公里", "辆", "小时"]
    title1 = "周测试时间-里程汇总"
    title2 = "投入车辆-平均每台车每天测试时长"
    line5 = draw_chart2(
        x_data_temp, y_data_static[2], y_data_static[3], y_axis[2], y_axis[3], unit[2], unit[3], title1)
    page.add(line5)
    make_snapshot(snapshot, line5.render(), "周测试时间-里程汇总.png")

    line6 = draw_chart2(
        x_data_temp, y_data_static[4], y_data_static[5], y_axis[4], y_axis[5], unit[4], unit[5], title2)
    page.add(line6)
    make_snapshot(snapshot, line6.render(), "投入车辆-平均每台车每天测试时长.png")

    page.render("result.html")
