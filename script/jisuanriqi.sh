#!/bin/bash

today=`date -d now +%Y-%m-%d`
echo ${today}

yesterday=`date -d yesterday +%Y-%m-%d`
echo ${yesterday}

daybeforeyesterday=`date -d "2 days ago" +%Y-%m-%d`
echo ${daybeforeyesterday}

thismonday=`date -d "last monday" +"%Y-%m-%d %H:%M:%S"`
year_of_thismonday=`date -d "${thismonday}" +%Y`
week_of_thismonday=`date -d "${thismonday}" +%V`
echo "本周周一为：${thismonday}， 是${year_of_thismonday}年第${week_of_thismonday}周"
fourteenweeksago=`date -d "${thismonday} 14 weeks ago" +"%Y-%m-%d %H:%M:%S"`
year_of_fourteenweeksago=`date -d "${fourteenweeksago}" +%Y`
week_of_fourteenweeksago=`date -d "${fourteenweeksago}" +%V`
echo "十四星期前的周一为：${fourteenweeksago}， 是${year_of_fourteenweeksago}年第${week_of_fourteenweeksago}周"

day=$fourteenweeksago

mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select year(begin_timestamp),  week(begin_timestamp,1),  count(distinct(vehicle_id)) as beijing_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('beijing_huanbao_park','beijing_haidian_jiaxiao') group by year(begin_timestamp),week(begin_timestamp,1) order by year(begin_timestamp), week(begin_timestamp, 1);"

