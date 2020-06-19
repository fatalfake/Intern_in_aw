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
    start_time_new=${start_time_raw/ /%20}
    end_time_new=${end_time_raw/ /%20}
    start_time="\"start_time\":\"$start_time_new\""
    end_time="\"end_time\":\"$end_time_new\""
    server="localhost:5555/update-by-name"
    #server="https://monitor.autowise.tech/bag-trans-log/update-by-name"
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
if echo ${names[7]} | grep -q '[0-9]' || echo ${names[7]} | grep -q 'default' || echo ${names[7]} | grep -q 'offline'
then 
    vehicle_id="${names[6]}_${names[7]}"
    if echo ${names[10]} | grep -q '[^a-z]'
    then
        park_id="${names[8]}_${names[9]}"
    else
        park_id="${names[8]}_${names[9]}_${names[10]}"
    fi
else
    vehicle_id="${names[6]}"
    if echo ${names[9]} | grep -q '[^a-z]'
    then
        park_id="${names[7]}_${names[8]}"
    else
        park_id="${names[7]}_${names[8]}_${names[9]}"
    fi
fi
echo vehicle_id: ${vehicle_id}
echo park_id: ${park_id}

start_hex_meta=`head -n 1000 $filename | grep -s -a time= | head -n 1`
IFS='=' read -r -a starts <<< $(echo $start_hex_meta)
i=0
while [ $i -lt 100  ]
do
    ((i++))
    if echo ${starts[i]} | grep -s -a -q time
    then
	break
    fi
done
((i++))
start_hex=`echo ${starts[i]} | cut -b 1-8`

end_hex_meta=`tail -n 200 $filename | grep -s -a time= | tail -n 1`
IFS='=' read -r -a ends <<< $(echo $end_hex_meta)
j=0
while [ $j -lt 100 ]
do
    ((j++))
    if echo ${ends[j]} | grep -s -a -q time
    then 
	break
    fi
done
((j++))
end_hex=`echo ${ends[j]} | cut -b 1-8`

start_time_stamp=`./hex_to_time.out ${start_hex}`
end_time_stamp=`./hex_to_time.out ${end_hex}`
start_time=`date -d @${start_time_stamp} "+%Y-%m-%d %H:%M:%S"`
end_time=`date -d @${end_time_stamp} "+%Y-%m-%d %H:%M:%S"`
echo start_time: ${start_time}
echo end_time: ${end_time}
a=${start_time_stamp}
b=${end_time_stamp}
duration=$(echo "${b} - ${a}" | bc)
duration=`echo $duration | awk '{print int($duration)}'` #+0.5四舍五入，不过似乎rosbag info采用的是向下取整
echo duration: ${duration} s

status=0
size=0

write_meta $filename $status $size $vehicle_id $park_id $duration "$start_time" "$end_time"

