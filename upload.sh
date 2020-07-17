SALVEIFS=$IFS

IFS=$(echo -en "\n\b")

function send_file(){
    echo "上传文件 $filename"
    str=$RANDOM
    sign=`echo -n awupload$str| md5sum | awk '{print $1}'`
    # curl 连接超时时间10秒 请求超时时间5秒 超时后最多重试3次
    info=`curl -H "Expect:" -F "file=@$filename" --connect-timeout 10 --max-time 5 --retry 3 https://monitor.autowise.tech/vehicle-pose/upload-route\?token=$sign\&str=$str`

    #对响应码进行判断
    if [ "$info" == "success" ];then
        echo "处理成功，$info"
    else
        echo "处理失败，$info"
    fi
}

function upload(){
    if [ ! -n "$1" ] ;then
        echo "请指定目录！"
        exit
    fi
    for file in `ls $1`; do
	#如果是目录，进入此目录后再次调用upload
        if [ -d $1"/"$file ];then
            upload $1"/"$file
	#不是目录SSS
        else
            if [ "${file##*.}"x = "json"x ];then
                filename="$1/$file"
                send_file $filename
                sleep 0.5s
            fi
        fi
    done
}

up_path=/opt/ros/kinetic/share/aw_global_planning/data
upload "$up_path"
IFS=$SAVEIFS