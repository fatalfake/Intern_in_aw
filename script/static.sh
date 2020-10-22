#!/bin/bash

thismonday=`date -d "last monday" +"%Y-%m-%d %H:%M:%S"`
# echo ${thismonday}
# date -d "${thismonday}" +%V
fourteenweeksago=`date -d "${thismonday} 14 weeks ago" +"%Y-%m-%d %H:%M:%S"`
# echo ${fourteenweeksago}
# date -d "${fourteenweeksago} " +%V
day=$fourteenweeksago
#sum
mysql -uaw_monitor -hszjjh-webserver.szjjh.autowise.ai -p123Qwe aw_monitor_system -e "select week(begin_timestamp), sum(takeover_times),sum(stop_times),sum(timestampdiff(MINUTE,begin_timestamp,end_timestamp))/60 as duration, sum(distance)/1000, count(distinct(vehicle_id)) from task_execution where begin_timestamp > '$day' group by year(begin_timestamp),week(begin_timestamp,1); " > result.sum

# bash week_vehicle.sh $day
# bash week_duration.sh $day
# bash week_distance.sh $day
# bash week_duration_avg.sh $day
