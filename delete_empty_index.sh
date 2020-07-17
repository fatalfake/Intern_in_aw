#!/bin/bash

input="{"data":[{"index":0},{"index":1,"points":[[325100.556601454,3462264.861553303],{"index":20},{"index":0},{"index":0},[325101.45999999999,3462262.39]"

echo $input | sed "s/{"index":[0-9]\+},//g"

# input=`cat $1`

# echo $input | sed 's/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g' > generated.json

# sed 's/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g' ${todo_file} > generated.json

# sed -i 's/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g' ${todo_file}


rm_emptyindex_path=/opt/ros/kinetic/share/aw_global_planning/data

function rm_emptyindex(){
    if [ ! -n "$1" ] ;then
        echo "请指定目录！"
        exit
    fi
    for file in `ls $1`; do
	#如果是目录，进入此目录后再次调用
        if [ -d $1"/"$file ];then
            upload $1"/"$file
	#不是目录SSS
        else
            if [ "${file##*.}"x = "json"x ];then
                todo_file="$1/$file"
                sed -i 's/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g' ${todo_file}
                sleep 0.5s
            fi
        fi
    done
}

rm_emptyindex "${rm_emptyindex_path}"