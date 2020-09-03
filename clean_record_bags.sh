#!/bin/bash

bag_dir=$1
# current_timestamp=`date +%s`
# for bag in `ls $1`;do
#  	IFS='_' read -r -a names <<< $(echo $bag)
# 	date_of_file="${names[2]}"
#  	IFS='-' read -r -a date_parts <<< $(echo $date_of_file)
# 	year="${date_parts[0]}"
# 	month="${date_parts[1]}"
# 	day="${date_parts[2]}"
# 	record_day=${year}-${month}-${day}
# 	t=`date -d "$record_day" +%s`
# 	delta=$((current_timestamp - t))
# 	if [ $delta -gt 259200 ];then
# 		echo "$record_day greater the 3days, remove $bag_dir/$bag"
# 		rm $bag_dir/$bag
		
# 	fi

# done

for bag_new in `ls $1`;do
 	IFS='_' read -r -a name <<< $(echo $bag_new)
	year_new="${name[0]}"
	month_new="${name[1]}"
	day_new="${name[2]}"
	record_day_new=${year_new}-${month_new}-${day_new}
	t_new=`date -d "$record_day_new" +%s`
	delta_new=$((current_timestamp - t_new))
	if [ $delta_new -gt 259200 ];then
		echo "$record_day_new greater the 3days, remove $bag_dir/$bag_new"
		rm $bag_dir/$bag_new		
	fi

done