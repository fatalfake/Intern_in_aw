#!/bin/bash
day=$1
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as beijing_duration, sum(distance)/1000 as beijing_distance,  count(distinct(vehicle_id)) as beijing_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('beijing_huanbao_park','beijing_haidian_jiaxiao') group by year(begin_timestamp),week(begin_timestamp,1);" > a
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as disney_duration, sum(distance)/1000 as disney_distance,  count(distinct(vehicle_id)) as disney_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_disney') group by year(begin_timestamp),week(begin_timestamp,1);" > b
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as auto_duration, sum(distance)/1000 as auto_distance,  count(distinct(vehicle_id)) as auto_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_auto_expo') group by year(begin_timestamp),week(begin_timestamp,1);" > c 
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as innovation_duration, sum(distance)/1000 as innovation_distance,  count(distinct(vehicle_id)) as innovation_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_innovation_port') group by year(begin_timestamp),week(begin_timestamp,1);" > d 
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as boschung_duration, sum(distance)/1000 as boschung_distance,  count(distinct(vehicle_id)) as boschung_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('boschung_factory') group by year(begin_timestamp),week(begin_timestamp,1);" >  e
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as walmart_duration, sum(distance)/1000 as walmart_distance,  count(distinct(vehicle_id)) as walmart_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('pheonix_walmart') group by year(begin_timestamp),week(begin_timestamp,1);" >  f
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as guangzhou_duration, sum(distance)/1000 as guangzhou_distance,  count(distinct(vehicle_id)) as guangzhou_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('guangzhou_longhu','guangzhou_longhu_all','guangzhou_longhu_park') group by year(begin_timestamp),week(begin_timestamp,1);" >  g
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as anting_duration, sum(distance)/1000 as anting_distance,  count(distinct(vehicle_id)) as anting_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('shanghai_anting_newtown') group by year(begin_timestamp),week(begin_timestamp,1);" >  h
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as songjiang_duration, sum(distance)/1000 as songjiang_distance,  count(distinct(vehicle_id)) as songjiang_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('hanghai_songjiang_test','shanghai_songjiang_xinfeng','shanghai_tuspark') group by year(begin_timestamp),week(begin_timestamp,1);" >  i
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp,1), year(begin_timestamp),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as suzhou_duration, sum(distance)/1000 as suzhou_distance,  count(distinct(vehicle_id)) as suzhou_vehicles from task_execution  where begin_timestamp>'$day' and park_id in ('suzhou_station_square','suzhou_xiangcheng_all','suzhou_xiangcheng_demo','suzhou_demo_park') group by year(begin_timestamp),week(begin_timestamp,1);" > j 



join -a1 -a2 a b|join -a1 -a2 - c|join -a1 -a2 - d|join -a1 -a2 - e|join -a1 -a2 - f|join -a1 -a2 - g|join -a1 -a2 - h|join -a1 -a2 - i|join -a1 -a2 - j > result

