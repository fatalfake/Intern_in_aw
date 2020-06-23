#!/usr/bin/env bash
#
# copy_bag_fastly.sh
# Copyright (C) 2018 yhshen <yhshen@yhshen-ThinkPad-P51>
#
# Distributed under terms of the MIT license.
#
# set -x 

MONITOR_DEVICE_PID="MONITOR_DEVICE_PID"
READ_DEVICE_PID="READ_DEVICE_PID"
MONITOR_DATA_PID="MONITOR_DATA_PID"
READ_DATA_PID="READ_DATA_PID"
SYNC_FILE_PID="SYNC_FILE_PID"

DEVICE_PATH_MONITORED="/media/";
BAG_PATH_MONITORED="/home/autowise/data/bags/";

printf "DEVICE_PATH_MONITORED: $DEVICE_PATH_MONITORED, BAG_PATH_MONITORED: $BAG_PATH_MONITORED\n"

INOTIFY_FIFO_EVENT_QUEUE="/var/copy_bag_device_monitor_queue.fifo"
INOTIFY_SRC_BAG_FIFO_EVENT_QUEUE="/var/src_bag_event_list.fifo"

MERGED_EVENT_QUEUE_LOCK_FILE="/var/lock/merged_event_queue.lock"
MERGED_EVENT_QUEUE="/var/merged_event_list.faked_fifo" 
SYNC_HISTORY_FILE_DEVICE_QUEUE_LOCK_FILE="/var/lock/sync_history_file_devices.lock"
SYNC_HISTORY_FILE_DEVICE_QUEUE="/var/sync_history_file_devices.faked_fifo"

# global variables used by add_or_delete_device_handler
declare -A DEVICE_PROCESS_PID
declare -A DEVICE_TIMESTAMP
LATEST_EVENT_TIME=0

#action functions
function move() {
    printf "action: $FUNCNAME, src: $1, dst: $2\n"
    mv $1 $2
    return $?
}

#input params: src_path dst_path
TOTAL_COPY_BW="200" #m: MB/s
function copy_to_device() {
    printf "action: $FUNCNAME, src: $1, dst: $2, bwlimit: $3\n"
    # -z: compress
    src_file=$1
    dst_file=$2
    bwlimit=$3

    if [[ ! -f ${src_file} ]]; then
        if [[ -f ${src_file%.active*} ]]; then
            move ${dst_file} ${dst_file%.active*}
            src_file=${src_file%.active*}
            dst_file=${dst_file%.active*}
            src_file_size=`stat -c "%s" $src_file`
            dst_file_size=`stat -c "%s" $dst_file`
            printf "bag $src_file ended, dst $dst_file, src size $src_file_size,  cur dst size $dst_file_size"
        fi        
    fi
    dst_dir=`dirname $2`
    if [[ ! -d "$dst_dir" ]]; then
        printf "$dst_dir not exists, create dir: $dst_dir\n"
        mkdir -p $dst_dir
    fi

    mounted_on=`df -h $dst_dir | tail -n 1 | awk '{print $6}'`
    if [[ "$mounted_on" == "/" ]]; then
        printf "dst_dir is mounted on /, ignore\n"
        return 0
    fi

    pgrep rosbag
    bag_recording=$?

    src_file_size=`stat -c "%s" $src_file`
    printf "action: $FUNCNAME, src_file: $src_file, dst_file: $dst_file, bwlimit: $bwlimit, recording: $bag_recording, src size: $src_file_size\n"

    if [[ $bag_recording -ne 0 || $# -eq 2 ]]; then
        /usr/bin/rsync -P --append-verify --info=progress2 $src_file $dst_file
        func_ret=$?
    else
        /usr/bin/rsync -P --append-verify --info=progress2 --bwlimit=${bwlimit}m $src_file $dst_file
        func_ret=$?
    fi
    src_file_size=`stat -c "%s" $src_file`
    dst_file_size=`stat -c "%s" $dst_file`
    printf "action complete: $FUNCNAME, src_file: $src_file, dst_file: $dst_file, bwlimit: $bwlimit, recording: $bag_recording, src size: $src_file_size, dst size: $dst_file_size, ret: $func_ret\n"
    return $func_ret
}

function direct_copy() {
    printf "action: $FUNCNAME, src: $1, dst: $2, bwlimit: $3\n"
    src_file=$1
    dst_file=$2

    if [[ ! -f ${src_file} ]]; then
        if [[ -f ${src_file%.active*} ]]; then
            move ${dst_file} ${dst_file%.active*}
            src_file=${src_file%.active*}
            dst_file=${dst_file%.active*}
            src_file_size=`stat -c "%s" $src_file`
            dst_file_size=`stat -c "%s" $dst_file`
            printf "bag $src_file ended, dst $dst_file, src size $src_file_size,  dst size $dst_file_size"
        fi        
    fi
    dst_dir=`dirname $2`
    if [[ ! -d "$dst_dir" ]]; then
        printf "$dst_dir not exists, create dir: $dst_dir\n"
        mkdir -p $dst_dir
    fi

    mounted_on=`df -h $dst_dir | tail -n 1 | awk '{print $6}'`
    if [[ "$mounted_on" == "/" ]]; then
        printf "dst_dir is mounted on /, ignore\n"
        return 0
    fi

    src_file_size=`stat -c "%s" $src_file`
    printf "action: $FUNCNAME begin rsync, src_file: $src_file, dst_file: $dst_file, bwlimit: $3, src size: $src_file_size\n"

    if [[ $# -eq 2 ]]; then
        /usr/bin/rsync -gpoltD --progress --append $src_file $dst_file
        func_ret=$?
    else
        /usr/bin/rsync -gpoltD --progress --append --bwlimit=$3m $src_file $dst_file
        func_ret=$?
    fi
    src_file_size=`stat -c "%s" $src_file`
    dst_file_size=`stat -c "%s" $dst_file`
    printf "action complete: $FUNCNAME end rsync, src_file: $src_file, dst_file: $dst_file, bwlimit: $3, src size: $src_file_size, dst size: $dst_file_size, ret: $func_ret\n"

    return $func_ret
}

function delete() {
    printf "action: $FUNCNAME, src: $1\n"
    rm $1
    return $?
}

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
    server="localhost:5555/update-by-name"
    # server="https://monitor.autowise.tech/bag-trans-log/update-by-name"
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

# input params: fifo_file_name handler path_prefix
function read_fifo_process() {
    local fifo_file_name=$1
    local handler=$2
    local block_time=""
    if [[ $# -ge 3 && "$3" == "-t" ]]; then
        block_time="-t $4"
        shift 4
    else
        shift 2
    fi

    printf "new process $$ function $FUNCNAME inparams: $*, \
after shift fifo_file_name: $fifo_file_name, handler: $handler, \$*: $*\n";

    while true; do
        # process one line one time
        # printf "$FUNCNAME => $line\n"
        read $block_time line
        $handler $line $@; 
    done < $fifo_file_name

    printf "read fifo process($handler) end\n"
}

#input params: path fifo_file event(optional)
function monitor_path() {
    printf "new process $$ function $FUNCNAME inparams: $*, inparams'num: $#\n";
    case $# in 
        3) 
            printf "to monitor path: $1, fifo file name: $2, listen events: $3\n";    
            inotifywait -m $1 --timefmt '%s' --format '%T %e %w%f' -e ${3//|/ -e } > $2
            ;;
        2)
            printf "to monitor path: $1, fifo file name: $2, no events given, listen all events\n";    
            inotifywait -m $1 --timefmt '%s' --format '%T %e %w%f' > $2
            ;;
        *)
            printf "monitor_path's input params num wrong($#), exited -1\n";
            return -1;
            ;;
    esac

    printf "monitor path end\n"
}

first_event_tm=""
prev_n_event_string=""
last_event=""
function process_file_notify_event() {
    cur_timestamp=`date +'%s'`
    # printf "$READ_DATA_PID cur_timestamp: $cur_timestamp, function $FUNCNAME inparams: \"$*\", inparams'num: $#\n";
    event_timestamp=$1
    event_type="file"
    event=$2
    event_object="$4$3"

    if [[ $# > 2 ]]; then
        IFS=',' read -r -a arr <<< "$event";
        case ${#arr[@]} in
            1 )
                # file inotify event
                event_type="file";
                event=${arr[0]};
                ;;
            2 ) 
                # dir inotify event
                event_type="dir";
                event=${arr[0]};
                ;;
            * )
                printf "recevied event_type: $event_type, inparams: \"$*\" unexpected\n"
                return -1;
        esac

        # printf "$READ_DATA_PID type: $event_type, event: $event, event_object: $event_object\n"

        #only sync file, not dirs
        if [[ "$event_type" == "file" ]]; then
            if [[ "${event_object: (-4)}" == ".bag" || "${event_object: (-7)}" == ".active" ]]; then
                if [[ "$prev_n_event_string" == "" ]]; then
                    prev_n_event_string="$event_timestamp|$event|$event_object;"
                    first_event_tm="$event_timestamp"
                    last_event="$event|$event_object"
                elif [[ "$last_event" == "$event|$event_object" ]]; then
                    # printf "$read_data_pid same event with prev event($event|$event_object), ignore"
                    true;
                else
                    prev_n_event_string="${prev_n_event_string}$event_timestamp|$event|$event_object;"
                    last_event="$event|$event_object"
                fi
            fi
        fi
    fi

    if [[ "$first_event_tm" == "" ]]; then
        # printf "return first_event_tm is empty, prev_n_event_string: ${prev_n_event_string}, last_event: ${last_event}"
        return 0;
    fi

    delta_t=`echo "$cur_timestamp - $first_event_tm" | bc`

    # printf "$read_data_pid cur prev_n_event_string: \"$prev_n_event_string\", delta_t: $delta_t\n"

    if [[ $delta_t -ge 5 ]]; then
        {
            flock -x 200
            echo $prev_n_event_string >> ${MERGED_EVENT_QUEUE}
        } 200>${MERGED_EVENT_QUEUE_LOCK_FILE}

        # printf "$read_data_pid put prev_n_event_string to ${MERGED_EVENT_QUEUE}\n"

        prev_n_event_string=""
        first_event_tm=""
        last_event=""
    fi
}

#input params: dst_path
function sync_history_files() {
    printf "function $FUNCNAME inparams: $*, inparams'num: $#\n"
    local device=$1
    local dst_path=$1
    local start_timestamp=$2
    local bwlimit=$3
    file_suffix_pattern="*.active|*.bag"
    #type_name_filter="-type f -name ${file_suffix_pattern/|/ -o -type f -name }"
    type_name_filter=$(echo ${file_suffix_pattern} | sed 's/|/ -o -type f -name /g')
    echo ${file_suffix_pattern} | sed 's/|/ -o -type f -name /g'
    printf "$SYNC_FILE_PID $FUNCNAME, src_path: $BAG_PATH_MONITORED, start_timestamp: `date '+%Y-%m-%d %H:%M:%S' -d @${start_timestamp}`, dst_path: $dst_path, type_name_filter: $type_name_filter\n"

    # find $BAG_PATH_MONITORED ! -path $BAG_PATH_MONITORED $type_name_filter -newermt "$(date '+%Y-%m-%d %H:%M:%S' -d @${start_timestamp})"
    for file in `find $BAG_PATH_MONITORED ! -path $BAG_PATH_MONITORED \( -type f -name '*.bag' -o -type f -name '*.active' \) -newermt "$(date '+%Y-%m-%d %H:%M:%S' -d @${start_timestamp})"`; do
        #maybe need to set background
        printf "$SYNC_FILE_PID to copy history file: $file\n"
        need_to_sync_now=false
        dst_file="$dst_path/`basename $file`"
        if [[ -f "${dst_file}.md5" ]]; then
            printf "$file has been copied to this device before, ignore\n"
            continue
        else
            {
                flock -x 200
                grep "$file" ${MERGED_EVENT_QUEUE}
                if [[ $? -ne 0 ]]; then
                    need_to_sync_now=true
                fi
            } 200>$MERGED_EVENT_QUEUE_LOCK_FILE
        fi

        if [[ "$need_to_sync_now" == "true" ]]; then
            direct_copy $file ${file/$BAG_PATH_MONITORED/"$dst_path/"}

            filename=`basename $file`

            if echo ${filename} | grep -q '.active'
            then
                echo "######################################"
                echo "####.active file detected. Ignore.####"
                echo "######################################"
            else
            
                IFS='_' read -r -a names <<< $(echo $filename)

                date_of_file="${names[0]}${names[1]}${names[2]}"
                vehicle_id="${names[6]}_${names[7]}"
                park_id="${names[8]}"
                n=9
                while [ $n -lt 100 ]
                do
                    park_id="${park_id}_${names[n]}"
                    ((n++))
                    if echo ${names[n]} | grep -q '.bag'
                    then
                        break
                    fi
                done
                echo vehicle_id: ${vehicle_id}
                echo park_id: ${park_id}

                head -n 1000 $file | grep -s -a time= | head -n 1 > start_hex_meta_temp.txt
                tail -n 200 $file | grep -s -a time= | tail -n 1 > end_hex_meta_temp.txt

                start_time_stamp=`/usr/local/lib/systemd/user/scripts/hex_to_time.out start_hex_meta_temp.txt`
                end_time_stamp=`/usr/local/lib/systemd/user/scripts/hex_to_time.out end_hex_meta_temp.txt`
                start_time=`date -d @${start_time_stamp} "+%Y-%m-%d %H:%M:%S"`
                end_time=`date -d @${end_time_stamp} "+%Y-%m-%d %H:%M:%S"`
                echo start_time: ${start_time}
                echo end_time: ${end_time}
                a=${start_time_stamp}
                b=${end_time_stamp}
                duration=$(echo "${b} - ${a}" | bc)
                duration=`echo $duration | awk '{print int($duration)}'` 
                echo duration: ${duration} s
            
                write_meta `basename $file` "Copied" `stat -c "%s" $file` $vehicle_id $park_id $duration $start_time_stamp $end_time_stamp
            fi
        fi
        printf "$SYNC_FILE_PID copy $file $dst_path complete\n"
    done

    #ret=1
    #while [[ true ]]; do
    #    remove_device $device
    #    ret=$?
    #    if [[ $ret -ne 0 ]]; then
    #        printf "remove device: $device failed\n"
    #        sleep 10
    #    else
    #        printf "remove device: $device success\n"
    #        notify-send -t 60000 \["同步bag成功"\] "请拔出移动硬盘"
    #        break
    #    fi
    #done
}

function sync_file_per_device() {
    printf "$SYNC_FILE_PID function $FUNCNAME inparams: $*\n";
    event_timestamp=$1
    event=$2
    event_object=$3
}

#input params: 
function sync_file() {
    printf "$SYNC_FILE_PID function $FUNCNAME inparams: $*\n";
    declare -a running_sync_processes
    declare -A device_pids
    declare -a device_root_paths;
    local event_string="";
    local device_info_buffer="";
    while [[ true ]]; do
        #sync device info
        {
            # printf "$SYNC_FILE_PID step into sync device info\n"
            flock -x 300
            device_info_buffer=$(cat $SYNC_HISTORY_FILE_DEVICE_QUEUE)
            # printf "$SYNC_FILE_PID device info buffer: $device_info_buffer\n"
            > $SYNC_HISTORY_FILE_DEVICE_QUEUE
        } 300>$SYNC_HISTORY_FILE_DEVICE_QUEUE_LOCK_FILE

        if [[ "$device_info_buffer" != "" ]]; then
            declare -a sync_history_devices;
            declare -a sync_timestamps;

            local action="MODIFY"
            local device="xxx"
            local sync_timestamp="";
            
            IFS='|' read -r -a arr <<< "$device_info_buffer";
            for device_info in ${arr[*]}; do
                # printf "$SYNC_FILE_PID device_info: $device_info\n"
                if [[ "$device_info" != "" ]]; then
                    IFS=';' read -r -a arr2 <<< "$device_info";
                    for key_value in ${arr2[*]}; do
                        IFS='=' read -r -a arr3 <<< "$key_value";
                        # printf "$SYNC_FILE_PID key_value: $key_value\n"
                        if [[ "$key_value" != "" ]]; then
                            # printf "$SYNC_FILE_PID key: ${arr3[0]}, value: ${arr3[1]}\n"
                            case "${arr3[0]}" in
                                "action")
                                    action="${arr3[1]}"
                                    ;;
                                "device")
                                    device="${arr3[1]}"
                                    ;;
                                "sync_timestamp")
                                    sync_timestamp="${arr3[1]}"
                                    ;;
                            esac
                        fi
                    done
                fi

                printf "$SYNC_FILE_PID action: $action, device: $device, sync_timestamp: $sync_timestamp\n"

                case "$action" in
                    "CREATE")
                        device_root_paths+=("$device");
                        if [[ "$sync_timestamp" != "" ]]; then
                            sync_history_devices+=("$device")
                            sync_timestamps+=("$sync_timestamp")
                        fi
                        ;;
                    "DELETE")
                        for i in ${!device_root_paths[*]}; do
                            if [[ "${device_root_paths[$i]}" == "$device" ]]; then
                                unset 'device_root_paths[$i]'
                            fi
                        done
                        for i in ${!sync_history_devices[*]}; do
                            if [[ "${sync_history_devices[$i]}" == "$device" ]]; then
                                unset 'sync_history_devices[$i]'
                                unset 'sync_timestamps[$i]'
                            fi
                        done
                        for i in ${!device_pids[*]}; do
                            if [[ "$i" == "$device" ]]; then
                                printf "device removed, to kill process copying to this device, $i => ${device_pids[$i]}\n"
                                sleep 5
                                kill ${device_pids[$i]} > /dev/null 2>&1
                                unset device_pids[$i]
                            fi
                        done
                        ;;
                esac

                printf "$SYNC_FILE_PID device_root_paths: ${device_root_paths[*]}\n"
                printf "$SYNC_FILE_PID sync_history_devices: ${sync_history_devices[*]}\n"
                printf "$SYNC_FILE_PID sync_timestamps: ${sync_timestamps[*]}\n"

                sync_timestamp=""
            done

            if [[ ${#sync_history_devices[*]} -ne 0 ]]; then
                local bwlimit=$(( $TOTAL_COPY_BW / ${#sync_history_devices[*]} ))
                for i in ${!sync_history_devices[*]}; do
                    if [[ "${sync_history_devices[$i]}" != "" && "${sync_timestamps[$i]}" != "" ]]; then
                        sync_history_files ${sync_history_devices[$i]} ${sync_timestamps[$i]} $bwlimit &
                        running_sync_processes+=($!)
                        device_pids+=(["${sync_history_devices[$i]}"]="$!")
                    fi
                done
            fi

            unset sync_history_devices
            unset sync_timestamps
        fi

        #sync inotify events
        {
            #printf "$SYNC_FILE_PID step into sync sync inotify events\n"
            flock -x 200
            local this_event_string=$(cat ${MERGED_EVENT_QUEUE} | tr -d '\n') 
            if [[ "$this_event_string" != "" ]]; then
                if [[ "$event_string" == "" ]]; then
                    event_string="${this_event_string}"
                else
                    event_string="${event_string};${this_event_string}"
                fi
            fi 
            #clear merged_event_queue file
            > ${MERGED_EVENT_QUEUE}
        } 200>$MERGED_EVENT_QUEUE_LOCK_FILE

        if [[ "$event_string" == "" ]]; then
            # printf "$SYNC_FILE_PID sync_file: no event, to sleep 3 seconds\n"
            sleep 3
        else
            # printf "$SYNC_FILE_PID merge all events: $event_string\n========================\n"
            IFS=';' read -r -a arr <<< $event_string;

            declare -a tms
            declare -a events
            declare -a objects

            merged_event_string=""
            printf "$SYNC_FILE_PID splited events' num: ${#arr[*]}\n"
            for event_info in ${arr[@]}; do
                # printf "event_info: $event_info, merged_event_string: $merged_event_string\n"
                if [[ "$event_info" != "" ]]; then
                    IFS='|' read -r -a temp <<< "$event_info";
                    #printf "event_info: $event_info, temp: ${temp[*]}\n"
                    local last_index=$(( ${#events[*]} - 1 ))
                    case ${temp[1]} in
                        "MOVED_TO")
                            local find_move_from="false"
                            if [[ ${#events[*]} -ne 0 ]]; then
                                merged_event_string="$merged_event_string;$event_info"
                                for i in `seq $last_index -1 0`; do
                                    if [[ "${events[$i]}" == "MOVED_FROM" ]]; then
                                        printf "merge MOVED_TO to MOVED_FROM\n"
                                        events[$i]="MOVED"
                                        objects[$i]="${objects[$i]} ${temp[2]}"
                                        find_move_from="true"
                                        break;
                                    fi
                                done
                            fi
                            if [[ "$find_move_from" == "false" ]]; then
                                printf "can't find MOVE_FROM event: $event_info, maybe a move operation\n"
                                tms+=(${temp[0]})
                                events+=(${temp[1]})
                                objects+=(${temp[2]})
                                merged_event_string="$merged_event_string;$event_info"
                            fi
                            ;;
                        *)
                            if [[ $last_index -lt 0 ]]; then
                                tms+=(${temp[0]})
                                events+=(${temp[1]})
                                objects+=(${temp[2]})
                                merged_event_string="$merged_event_string;$event_info"
                            else
                                #printf "last_index: $last_index,  events[last_index]: ${events[$last_index]}, temp[1]: ${temp[1]}, objects[last_index]: ${objects[$last_index]}, temp[2]: ${temp[2]}\n"
                                if [[ "${objects[$last_index]}" != "${temp[2]}" ]]; then
                                    tms+=(${temp[0]})
                                    events+=(${temp[1]})
                                    objects+=(${temp[2]})
                                    merged_event_string="$merged_event_string;$event_info"
                                else
                                    if [[ "${events[$last_index]}" != "${temp[1]}" ]]; then
                                        printf "events diff, last: ${events[$last_index]}, cur: ${temp[1]}\n"
                                        events[$last_index]="${temp[1]}"
                                        merged_event_string="$merged_event_string;$event_info"
                                    fi
                                fi
                            fi
                            ;;
                    esac
                fi
            done
            # printf "$SYNC_FILE_PID after merge, tms'num: ${#tms[*]}, events'num: ${#events[*]}, objects's num: ${#objects[*]}\n"
            for i in ${!events[*]}; do
                printf "index: $i, event: ${events[$i]}, event_time: ${tms[$i]}, object: ${objects[$i]}\n"
            done

            local ret=-1
            if [[ ${#running_sync_processes[*]} -ne 0 ]]; then
                ps -p ${running_sync_processes[*]} > /dev/null
                ret=$?
                printf "$SYNC_FILE_PID to checking process: ${running_sync_processes[*]}, ret: ${ret}\n"
            else
                printf "$SYNC_FILE_PID no running backgroud process, can sync new events\n"
            fi

            if [[ $ret -eq 0 ]]; then
                event_string=$merged_event_string;
                # printf "$SYNC_FILE_PID previous sync file process not terminated, sync next time, event_string: $event_string, to sleep 3 seconds\n"
                sleep 3
            elif [[ ${#device_root_paths[*]} -eq 0 ]]; then
                event_string=""

                for key in ${!running_sync_processes[*]}; do
                    # printf "prepare to unset sync process: $key => ${running_sync_processes[$key]}\n"
                    unset running_sync_processes[$key]
                done
                for key in ${!device_pids[*]}; do
                    # printf "prepare to unset device sync process: $key => ${device_pids[$key]}\n"
                    unset device_pids[$key]
                done

                # printf "$SYNC_FILE_PID no device need to sync file\n"
            else
                event_string=""

                for key in ${!running_sync_processes[*]}; do
                    # printf "prepare to unset sync process: $key => ${running_sync_processes[$key]}\n"
                    unset running_sync_processes[$key]
                done
                for key in ${!device_pids[*]}; do
                    # printf "prepare to unset device sync process: $key => ${device_pids[$key]}\n"
                    unset device_pids[$key]
                done

                # printf "$SYNC_FILE_PID prepare to sync file\n"
                for i in ${!events[*]}; do
                    cur_tm=`date +'%s'`
                    delta_tm=$(( $cur_tm - ${tms[$i]} ))
                    printf "index: $i, event: ${events[$i]}, cur_tm: $cur_tm, event_time: ${tms[$i]}, delta_tm: $delta_tm\n"
                    if [[ "${events[$i]}" == "MOVED_FROM" && $delta_tm -lt 120 ]]; then
                        event_string="${tms[$i]}|MOVED_FROM|${objects[$i]}\n";
                    else
                        local device_nums=${#device_root_paths[*]}
                        local bw_limit=$(( $TOTAL_COPY_BW / $device_nums ))

                        printf "total_copy_bw: $TOTAL_COPY_BW, device_nums: $device_nums\n"
                        for dst_root_path in ${device_root_paths[*]}; do
                            if [[ "${events[$i]}" == "MODIFY" || "${events[$i]}" == "MOVED_TO" ]]; then
                                copy_to_device ${objects[$i]} ${objects[$i]/${BAG_PATH_MONITORED}/"${dst_root_path}/"} $bw_limit &
                                running_sync_processes+=($!)
                                device_pids+=(["$dst_root_path"]="$!")
                            elif [[ "${events[$i]}" == "DELETE" ]]; then
                                printf "object: ${objects[$i]}, \
                                    src_root: ${BAG_PATH_MONITORED}, \
                                    dst_root: ${dst_root_path}\n"

                                delete ${objects[$i]/${BAG_PATH_MONITORED}/"${dst_root_path}/"}
                                #running_sync_processes+=($!)
                                #device_pids+=(["$dst_root_path"]="$!")
                            elif [[ "${events[$i]}" == "MOVED" ]]; then
                                IFS=' ' read -r -a temp <<< "${objects[$i]}";
                                printf "temp: ${temp[0]}, ${temp[1]}, src_root: ${BAG_PATH_MONITORED}, dst_root: ${dst_root_path}\n"
                                move ${temp[0]/${BAG_PATH_MONITORED}/"${dst_root_path}/"} ${temp[1]/${BAG_PATH_MONITORED}/"${dst_root_path}/"}

                                while [[ true ]]; do
                                    copy_to_device ${temp[1]} ${temp[1]/${BAG_PATH_MONITORED}/"${dst_root_path}/"} $bw_limit &
                                    if [[ $? -eq 0 ]]; then
                                        break;
                                    fi 
                                    sleep 60
                                done
                                running_sync_processes+=($!)
                                device_pids+=(["$dst_root_path"]="$!")
                            elif [[ "${events[$i]}" == "MOVED_FROM" ]]; then
                                if [[ -f ${objects[$i]%.active*} ]]; then
                                    bag_file_name=${objects[$i]%.active*} 
                                    move ${objects[$i]/${BAG_PATH_MONITORED}/"${dst_root_path}/"} ${bag_file_name/${BAG_PATH_MONITORED}/"${dst_root_path}/"}

                                    while [[ true ]]; do
                                        printf "add another sync operation: $bag_file_name => ${bag_file_name/${BAG_PATH_MONITORED}/"${dst_root_path}/"}\n"
                                        copy_to_device $bag_file_name ${bag_file_name/${BAG_PATH_MONITORED}/"${dst_root_path}/"} $bw_limit &
                                        if [[ $? -eq 0 ]]; then
                                            break;
                                        fi 
                                        sleep 60
                                    done
                                    running_sync_processes+=($!)
                                    device_pids+=(["$dst_root_path"]="$!")
                                fi
                            else 
                                printf "$SYNC_FILE_PID event: ${events[$i]}, ignore\n"
                            fi
                        done

                        # wait this event to complete
                        wait 
                    fi
                done
            fi

            unset tms;
            unset events;
            unset objects;
       fi
    done
}

#input params: device_path
function add_monitor_process_per_device() {
    printf "$READ_DEVICE_PID function $FUNCNAME inparams: $*\n";
    device=`readlink -f $1`
    device_add_timestamp=$2

    declare -A config

    config=(["device"]="$device" ["start_timestamp"]=21600 ["sync_history"]="false")
    #filesystem latency ????
    sleep 5

    device_label=`df -h |grep $device`
    if [[ "$device_label" == "" ]]; then
        printf "$device mounts with no device, so skipped\n"
    fi

    if [[ -f "$device/backup/sync" ]]; then
        printf "$READ_DEVICE_PID $device/backup/sync exists, so use config from it\n"
        while read line; do
            local key_value=`echo $line | sed 's/ //g' | tr [A-Z] [a-z]`; 
            printf "read sync config new line: $line, key_value: $key_value\n"
            IFS='=' read -r -a arr <<< "$key_value";
            if [[ "${arr[0]}" == "$device" ]]; then
                printf "can't config device path\n"
                continue;
            fi
            config[${arr[0]}]="${arr[1]}"
        done < "$device/backup/sync"
    elif [[ -f "$device/sync" ]]; then
        printf "$READ_DEVICE_PID $1/sync exists, so use config from it\n";
        cat "$device/sync"
        while read line; do
            local key_value=`echo $line | sed 's/ //g' | tr [A-Z] [a-z]`; 
            printf "read sync config new line: $line, key_value: $key_value\n"
            IFS='=' read -r -a arr <<< "$key_value";
            if [[ "${arr[0]}" == "$device" ]]; then
                printf "can't config device path\n"
                continue;
            fi
            config[${arr[0]}]="${arr[1]}"
        done < "$device/sync"
    else
        printf "$READ_DEVICE_PID no sync flag file found, so ignore this device: $1\n"
        unset config
        return 0
    fi
    
    # print config
    local device_timestamp_buffer="|action=CREATE"
    for key in $(echo ${!config[*]}); do
        printf "$READ_DEVICE_PID config key: $key, value: ${config[$key]}\n"
        device_timestamp_buffer="$device_timestamp_buffer;$key=${config[$key]}"
    done

    if [[ "${DEVICE_TIMESTAMP[$device]}" == "" ]]; then
        # not add before
        if [[ "${config["sync_history"]}" == "true" ]]; then
            printf "$READ_DEVICE_PID sync_history flag is set, will sync history files next loop\n"
            sync_timestamp=`echo $2 - ${config["start_timestamp"]} | bc`
            device_timestamp_buffer="$device_timestamp_buffer;sync_timestamp=$sync_timestamp"
        fi

        printf "device_timestamp_buffer: $device_timestamp_buffer\n"
        {
            flock -x 300
            echo $device_timestamp_buffer >>$SYNC_HISTORY_FILE_DEVICE_QUEUE
        } 300>$SYNC_HISTORY_FILE_DEVICE_QUEUE_LOCK_FILE

        printf "$READ_DEVICE_PID add device to dict: $1 => $2\n"
        DEVICE_TIMESTAMP+=(["$device"]="$device_add_timestamp")
    else
        printf "$READ_DATA_PID device: has exists before, no need to re-add\n"
    fi
    unset config
}

# input params: timestamp event name path_prefix
function add_or_delete_device_handler() {
    cur_timestamp=`date +'%s'`
    printf "$READ_DEVICE_PID cur_timestamp: $cur_timestamp, function $FUNCNAME inparams: \"$*\", inparams'num: $#\n";

    OLD_IFS=$IFS
    IFS=","
    arr=($2)
    IFS=$OLD_IFS

    printf "$READ_DEVICE_PID type: ${arr[0]}, event: ${arr[1]}, device: $4$3\n"
    if [[ ${arr[1]} == "ISDIR" ]]; then
        if [[ ${arr[0]} == "CREATE" ]]; then
            add_monitor_process_per_device "$4$3" "$1"
        elif [[ ${arr[0]} == "DELETE" ]]; then
            for key in $(echo ${!DEVICE_TIMESTAMP[*]}); do
                if [[ $key == "$4$3" && "${DEVICE_TIMESTAMP[$key]}" != "" ]]; then
                    unset DEVICE_TIMESTAMP[$key]
                    {
                        flock -x 300
                        echo "|action=DELETE;device=$4$3" >>$SYNC_HISTORY_FILE_DEVICE_QUEUE
                    } 300>$SYNC_HISTORY_FILE_DEVICE_QUEUE_LOCK_FILE
                fi
            done
        else
            printf "$READ_DEVICE_PID unexpexted event type: ${arr[0]}\n"
        fi
    fi
}

function test() {
    sleep $1 
    printf "test sleep $1"
}

function main() {
    #add existed device monitor
    local cur_timestamp=`date +'%s'`
    for path in `df -h | grep ${DEVICE_PATH_MONITORED} | awk '{print $6}'`; do
        add_monitor_process_per_device $path $cur_timestamp
    done
    
    # delete fifo file last time
    rm -f $MERGED_EVENT_QUEUE
    rm -f $INOTIFY_FIFO_EVENT_QUEUE 
    rm -f $INOTIFY_SRC_BAG_FIFO_EVENT_QUEUE
    
    touch $MERGED_EVENT_QUEUE
    touch $SYNC_HISTORY_FILE_DEVICE_QUEUE
    mkfifo $INOTIFY_FIFO_EVENT_QUEUE
    mkfifo $INOTIFY_SRC_BAG_FIFO_EVENT_QUEUE

    sync_file &
    SYNC_FILE_PID=$!
    printf "sync_file PID: $SYNC_FILE_PID\n"

    # monitor device dirs
    read_fifo_process $INOTIFY_FIFO_EVENT_QUEUE "add_or_delete_device_handler" & 
    READ_DEVICE_PID=$!
    printf "read device PID: $READ_DEVICE_PID\n"

    read_fifo_process $INOTIFY_SRC_BAG_FIFO_EVENT_QUEUE "process_file_notify_event" -t 5 & 
    READ_DATA_PID=$!
    printf "read data event PID: $READ_DATA_PID\n"

    monitor_path "-r ${DEVICE_PATH_MONITORED}" ${INOTIFY_FIFO_EVENT_QUEUE} 'create|delete' &
    MONITOR_DEVICE_PID=$!
    printf "monitor device PID: $MONITOR_DEVICE_PID\n" 

    monitor_path "-r ${BAG_PATH_MONITORED}" ${INOTIFY_SRC_BAG_FIFO_EVENT_QUEUE} 'modify|delete|close_write|move' &
    MONITOR_DATA_PID=$!
    printf "monitor src data PID: $MONITOR_DATA_PID\n"

    wait

    printf "main end\n"

    rm -f $MERGED_EVENT_QUEUE
    rm -f $SYNC_HISTORY_FILE_DEVICE_QUEUE
    rm -f $INOTIFY_FIFO_EVENT_QUEUE 
    rm -f $INOTIFY_SRC_BAG_FIFO_EVENT_QUEUE
}

main $@

