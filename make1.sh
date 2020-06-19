function write_meta() {
    printf "action: $FUNCNAME, src $1, status: $2, size: $3, vehicle_id: $4, park_id: $5, duration: $6, start_time: $7, end_time: $8"
    bagname="\"bag_name\":\"$1\""
    bagstatus="\"bag_status\":\"$2\""
    bagsize="\"bag_size\":\"$3\""
    #address_info="\"address\":\"$ADDRESS\""
    #other="\"other_info\":{$address_info}"
    vehicle_id="\"vehicle_id\":\"$4\""
    park_id="\"park_id\":\"$5\""
    duration="\"duration\":\"$6\""

    start_time_raw=`echo "$7"`
    end_time_raw=`echo "$8"`
    start_time="\"start_time\":\"$start_time_raw\""
    end_time="\"end_time\":\"$end_time_raw\""
    # server="localhost:5555/update-by-name"
    server="https://monitor.autowise.tech/bag-trans-log/update-by-name"
    json_args="{$bagname,$bagstatus,$bagsize,$vehicle_id,$park_id,$duration,$start_time,$end_time,\"insert_if_not_exist\":\"true\"}"
    echo $json_args

    retry=3
    while [[ $retry -ne 0 ]]; do
       ret=`curl -H "Content-type: application/json" -X POST -d $json_args $server | python -c "import sys, json; print json.load(sys.stdin)[\"status\"]"`
        if [[ "$ret" == "success" ]]; then
            return 0
        else
           retry=$(($retry - 1))
        fi
    done
    return -1
}

filename=$1

IFS='_' read -r -a names <<< $(echo $filename)

date_of_file="${names[0]}${names[1]}${names[2]}"
vehicle_id="${names[6]}_${names[7]}"
park_id="${names[8]}"
n=9
while [ $n -lt 100 ]
do
    park_id="${park_id}_${names[n]}"
    ((n++))
    if echo ${names[n]} | grep -q '[^a-z]'
    then
        break
    fi
done

echo vehicle_id: ${vehicle_id}
echo park_id: ${park_id}

head -n 1000 $filename | grep -s -a time= | head -n 1 > start_hex_meta_temp.txt

tail -n 200 $filename | grep -s -a time= | tail -n 1 > end_hex_meta_temp.txt


start_time_stamp=`./hex_to_time.out start_hex_meta_temp.txt`
end_time_stamp=`./hex_to_time.out end_hex_meta_temp.txt`
echo ${start_time_stamp}
echo ${end_time_stamp}
start_time=`date -d @${start_time_stamp} "+%Y-%m-%d %H:%M:%S"`
end_time=`date -d @${end_time_stamp} "+%Y-%m-%d %H:%M:%S"`
echo start_time: ${start_time}
echo end_time: ${end_time}
a=${start_time_stamp}
b=${end_time_stamp}
duration=$(echo "${b} - ${a}" | bc)
duration=`echo $duration | awk '{print int($duration)}'` #+0.5四舍五入，不过似乎rosbag info采用的是向下取整
echo duration: ${duration} s

status=Copied
size=`ls -l $filename| awk '{print $5}'`

write_meta $filename $status $size $vehicle_id $park_id $duration $start_time_stamp $end_time_stamp

