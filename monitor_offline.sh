#!/bin/bash
export LANG=C

# export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:/home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:/usr/bin:/usr/sbin:/bin:/sbin
# export LD_LIBRARY_PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib:/opt/ros/kinetic/lib:/opt/ros/kinetic/lib/x86_64-linux-gnu:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib

source /opt/ros/kinetic/setup.sh

# 定义MD5文件保存的路径
md5_path=./task_to_json_md5.sum
path=/opt/ros/kinetic/share/aw_launch/config/conf/tasks

source ~/.autowise/setup.sh
bash -c 'roscore' &
sleep 3
# gnome-terminal -x bash -c "roscore"
# xterm -e bash -c 'roscore' &

# 判断MD5基准校验文件是否存在，不存在则创建此文件,并修改权限只有root用户或者指定用户有读写权限
[ ! -f $md5_path ] && touch $md5_path && chmod 600 $md5_path
# read -p "请输入你需要MD5检验文件的目录,必须以\"/\"开始: " path
# if ! echo $path | grep -q "^/";then echo "不是以\"/\"开始";exit 1;fi

echo "#################################################"
# 如果所给需要md5校验的目录不存在，或者目录存在但目录下没有一个文件，则返回错误并提示用户
    if [ ! -d $path ] || [ `find $path -type f | wc -l` -eq 0 ];then
        echo "Error：Path does not exist or there is no file under the path"
        exit 1
    fi
# 判断基准文件数量
for i in `cat ${md5_path} | awk '{print $2}'`;do
    if [[ ! -f $i ]];then
        echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$i] \033[31m[MD5 check result：Removed]\033[0m" 2>&1
        # 去除md5 从$md5_path
        v2=${i//\//\\/}
        sed -i "${v2}/d" ${md5_path}
    fi
done

target_path=/opt/ros/kinetic/share/aw_global_planning/data

function set_env_and_launch(){
    sed -i 's/\r//g' ${list}
    export PLANNING_TASK=${list}
    echo 'Task file is: '${PLANNING_TASK}
    project_name=`echo ${list} | cut -d / -f 10`
    echo 'Project name is: '${project_name}
    rosrun aw_launch aw_config.py --cfg buss2 ${project_name}
    # gnome-terminal -x bash -c "source ~/.autowise/setup.sh;roslaunch aw_hdmap hdmap_runtime_env.launch"
    bash -c "source ~/.autowise/setup.sh;roslaunch aw_hdmap hdmap_runtime_env.launch" &
    #必须等待较长一段时间，否则hdmap会出错，未来可能会等更长时间
    sleep 60
    # source ./devel/setup.sh
    source ~/.autowise/setup.sh
    roslaunch aw_global_planning route_points_generator.launch
    if [ -f "${target_path}/fake.json" ]; then
        rm ${target_path}/fake.json
    fi
    park_id=`grep -E -o "park_id.+" ${list} | rev | cut -d ' ' -f1 | rev`
    route_id=`grep -E -o "route_id.+" ${list} | rev | cut -d ' ' -f1 | rev`
    echo -e "\033[32m${park_id}\033[0m"
    echo -e "\033[32m${route_id}\033[0m"
    jsonname=${park_id}_${route_id}
    jsonfile=`ls -t ${target_path} | grep ${jsonname} | head -n 1`
    echo -e "\033[32m${jsonname}\033[0m"
    echo -e "\033[32m${jsonfile}\033[0m"
    task_id=`basename ${list} | cut -d '.' -f1`
    sed -i "s%\"route_id\":\"[0-9]\+\"}%\"route_id\":\"${route_id}\",\"task_id\":\"${task_id}\"}%g" ${target_path}/${jsonfile}
    sed -i 's/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g' ${target_path}/${jsonfile}
    echo 'Task complete.'
}

for list in `find $path -type f`;do
    new_md5_arg1=`md5sum $list | awk '{print $1}'`
    new_md5_arg2=`md5sum $list | awk '{print $2}'`
    old_md5_arg2=`awk -v List="$list" '$2 == List{print $2}' $md5_path`
    if [[ "$new_md5_arg2" == "$old_md5_arg2" ]];then
        old_md5_arg1=`awk -v List="$list" '$2 == List{print $1}' $md5_path`
        if [[ ! "$new_md5_arg1" == "$old_md5_arg1" ]];then
            echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[31m[MD5 check result：Changed]\033[0m" 2>&1 
            # 去除原md5，添加新md5至 $md5_path
            v3=${list//\//\\/}
            sed -i "${v3}/d" ${md5_path}
            md5sum ${list} >> ${md5_path}
            # 执行JSON生成
            if [ "${list##*.}"x = "yaml"x ];then
                grep ${list} ./blacklist >> /dev/null
                if [ $? -ne 0 ];
                then
                    set_env_and_launch
                fi
            fi
        else
            echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[32m[MD5 check result：Unchanged]\033[0m"
        fi
    else
        new_file_md5=`md5sum ${list}`
        md5sum ${list} >> ${md5_path}
        echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[31m[MD5 check result：Added]\033[0m" 2>&1 
        if [ "${list##*.}"x = "yaml"x ];then
            grep ${list} ./blacklist >> /dev/null
            if [ $? -ne 0 ];
            then 
                set_env_and_launch
            fi
        fi
    fi
    # 如果文件数量大，可以把sleep的时间间隔设置小点。
    sleep 0.2
done


function send_file(){
    echo "上传文件 $1"
    str=$RANDOM
    sign=`echo -n awupload$str| md5sum | awk '{print $1}'`
    # curl 连接超时时间10秒 请求超时时间5秒 超时后最多重试3次
    info=`curl -H "Expect:" -F "file=@$1" --connect-timeout 10 --max-time 5 --retry 3 localhost:5555`

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
	#如果是目录，进入此目录后再次调用
        if [ -d $1"/"$file ];then
            upload $1"/"$file
	#不是目录SSS
        else
            if [ "${file##*.}"x = "json"x ];then
                todo_file="$1/$file"
                # sed -i 's/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g' ${todo_file}
                send_file ${todo_file}
                sleep 0.5s
            fi
        fi
    done
}

upload "${target_path}"
