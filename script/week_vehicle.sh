#!/bin/bash
#Ubuntu默认monday表示下一个周一，last monday表示上一个周一（通常是本周一,只有当前为周一时为上周一）
thismonday=`date -d "last monday" +"%Y-%m-%d %H:%M:%S"`
# echo ${thismonday}
# date -d "${thismonday}" +%V
fourteenweeksago=`date -d "${thismonday} 14 weeks ago" +"%Y-%m-%d %H:%M:%S"`
# echo ${fourteenweeksago}
# date -d "${fourteenweeksago} " +%V
day=$fourteenweeksago

mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as beijing_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('beijing_huanbao_park','beijing_haidian_jiaxiao') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" > a
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as disney_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_disney') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" > b
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as auto_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_auto_expo') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" > c 
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as innovation_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_innovation_port') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" > d 
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as boschung_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('boschung_factory') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" >  e
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as walmart_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('pheonix_walmart') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" >  f
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as guangzhou_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('guangzhou_longhu','guangzhou_longhu_all','guangzhou_longhu_park') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" >  g
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as anting_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_anting_newtown') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" >  h
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as songjiang_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('hanghai_songjiang_test','shanghai_songjiang_xinfeng','shanghai_tuspark') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" >  i
# mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select yearweek(begin_timestamp,1),  count(distinct(vehicle_id)) as suzhou_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('suzhou_station_square','suzhou_xiangcheng_all','suzhou_xiangcheng_demo','suzhou_demo_park') group by year(begin_timestamp),week(begin_timestamp,1) order by yearweek(begin_timestamp,1);" > j 



# python union.py a b c d e f g h i j |sort > result.vehicle