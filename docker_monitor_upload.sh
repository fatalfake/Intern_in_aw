#!/bin/bash

export LANG=C

# Add registry cert.
# export LC_ALL=C
# export DOMAIN_NAME=registry.autowise.ai
# openssl s_client -connect $DOMAIN_NAME:443 -showcerts </dev/null 2>/dev/null | openssl x509 -outform PEM | sudo tee /usr/local/share/ca-certificates/$DOMAIN_NAME.crt
# sudo update-ca-certificates

# echo "${HOME}/.coredump/core.%t.%e.%p" | sudo tee /proc/sys/kernel/core_pattern

# mkdir -p $HOME/.coredump

# docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined --network host --privileged -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar bash -c 'export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:/home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:/usr/bin:/usr/sbin:/bin:/sbin; update_autowise_debs_and_models.sh; cd qyc; ./monitor.sh'

# docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=localhost:10" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined --network host --privileged -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar bash -c 'export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:/home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:/usr/bin:/usr/sbin:/bin:/sbin; update_autowise_debs_and_models.sh; cd qyc; ./monitor.sh'

# xterm -e bash -c 'docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined --network host --privileged -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar:latest /bin/bash' &

# xterm -e bash -c 'docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=localhost:10.0" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined --network host --privileged -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar /bin/bash' &

docker pull registry.autowise.ai/awcar:latest

container=`docker ps | grep route_generator_docker | head -n 1 | cut -d ' ' -f1`

if [[ ! -n ${container} ]]; 
then 
    bash -c '
    docker run -ti --rm --name route_generator_docker\
    -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
    -v ~:/home/autowise/ \
    -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged=true \
    registry.autowise.ai/awcar:latest bash -c "cd qyc; ./monitor_offline.sh"'
fi

# sleep 5

# container=`docker ps | grep registry.autowise.ai/awcar | head -n 1 | cut -d ' ' -f1`


# docker exec -it ${container} bash -c 'export PATH=PATH:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
# /opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
# /sbin:/bin:/usr/bin:/usr/sbin; \
# export LANG=C
# export LD_LIBRARY_PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib:/opt/ros/kinetic/lib:/opt/ros/kinetic/lib/x86_64-linux-gnu:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib
# source /opt/ros/kinetic/setup.sh
# md5_path=/home/autowise/qyc/task_to_json_md5.sum
# path=/opt/ros/kinetic/share/aw_launch/config/conf/tasks
# source ~/.autowise/setup.sh
# [ ! -f $md5_path ] && touch $md5_path && chmod 600 $md5_path
# echo "#################################################"
#     if [ ! -d $path ] || [ `find $path -type f | wc -l` -eq 0 ];then
#         echo "Error：Path does not exist or there is no file under the path"
#         exit 1
#     fi
# for i in `cat ${md5_path} | awk '\''{print $2}'\''`;do
#     if [[ ! -f $i ]];then
#         echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$i] \033[31m[MD5 check result：Removed]\033[0m" 2>&1
#         v2=${i//\//\\/}
#         sed -i "${v2}/d" ${md5_path}
#     fi
# done
# for list in `find $path -type f`;do
#     new_md5_arg1=`md5sum $list | awk '\''{print $1}'\''`
#     new_md5_arg2=`md5sum $list | awk '\''{print $2}'\''`
#     old_md5_arg2=`awk -v List="$list" '\''$2 == List{print $2}'\'' $md5_path`
#     if [[ "$new_md5_arg2" == "$old_md5_arg2" ]];then
#         old_md5_arg1=`awk -v List="$list" '\''$2 == List{print $1}'\'' $md5_path`
#         if [[ ! "$new_md5_arg1" == "$old_md5_arg1" ]];then
#             echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[31m[MD5 check result：Changed]\033[0m" 2>&1 
#             v3=${list//\//\\/}
#             sed -i "${v3}/d" ${md5_path}
#             md5sum ${list} >> ${md5_path}
#             if echo ${list} | grep -q "\.yaml" 
#             then
#                 grep ${list} /home/autowise/qyc/blacklist >> /dev/null
#                 if [ $? -ne 0 ];
#                 then 
#                     export PLANNING_TASK=${list}
#                     echo "Task file is: "${PLANNING_TASK}
#                     project_name=`echo ${list} | cut -d / -f 10`
#                     echo "Project name is: "${project_name}
#                     rosrun aw_launch aw_config.py --cfg buss2 ${project_name}
#                     xterm -e bash -c "roscore" &
#                     xterm -e bash -c "source ~/.autowise/setup.sh;roslaunch aw_hdmap hdmap_runtime_env.launch" &
#                     sleep 60
#                     source ~/.autowise/setup.sh
#                     roslaunch aw_global_planning route_points_generator.launch
#                     echo "Task complete."
#                 fi
#             fi
#         else
#             echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[32m[MD5 check result：Unchanged]\033[0m"
#         fi
#     else
#         new_file_md5=`md5sum ${list}`
#         md5sum ${list} >> ${md5_path}
#         echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[31m[MD5 check result：Added]\033[0m" 2>&1 
#         if echo ${list} | grep -q "\.yaml" 
#         then
#             grep ${list} /home/autowise/qyc/blacklist >> /dev/null
#             if [ $? -ne 0 ];
#             then 
#                 export PLANNING_TASK=${list}
#                 echo "Task file is: "${PLANNING_TASK}
#                 project_name=`echo ${list} | cut -d / -f 10`
#                 echo "Project name is: "${project_name}
#                 rosrun aw_launch aw_config.py --cfg buss2 ${project_name}
#                 xterm -e bash -c "source ~/.autowise/setup.sh;roslaunch aw_hdmap hdmap_runtime_env.launch" &
#                 sleep 60
#                 source ~/.autowise/setup.sh
#                 roslaunch aw_global_planning route_points_generator.launch
#                 echo "Task complete."
#             fi
#         fi
#     fi
#     sleep 0.2
# done
# rm_emptyindex_path=/opt/ros/kinetic/share/aw_global_planning/data
# function rm_emptyindex(){
#     if [ ! -n "$1" ] ;then
#         echo "请指定目录！"
#         exit
#     fi
#     for file in `ls $1`; do
#         if [ -d $1"/"$file ];then
#             rm_emptyindex $1"/"$file
#         else
#             if [ "${file##*.}"x = "json"x ];then
#                 todo_file="$1/$file"
#                 sed -i "s/{"index":[0-9]\+},//g; s/,{"index":[0-9]\+}//g" ${todo_file}
#                 sleep 0.5s
#             fi
#         fi
#     done
# }

# rm_emptyindex "${rm_emptyindex_path}"

# SALVEIFS=$IFS

# IFS=$(echo -en "\n\b")

# function send_file(){
#     echo "上传文件 $list"
#     # str=$RANDOM
#     # sign=`echo -n awupload$str| md5sum | awk '\''{print $1}'\''`
#     # info=`curl -H "Expect:" -F "file=@$list" --connect-timeout 10 --max-time 5 --retry 3 https://monitor.autowise.tech/vehicle-pose/upload-route\?token=$sign\&str=$str`
#     # if [ "$info" == "success" ];then
#     #     echo "处理成功，$info"
#     # else
#     #     echo "处理失败，$info"
#     # fi
# }
# md5_path=/home/autowise/qyc//json_md5_log.sum
# path=/opt/ros/kinetic/share/aw_global_planning/data
# [ ! -f $md5_path ] && touch $md5_path && chmod 600 $md5_path
# echo "#################################################"
#     if [ ! -d $path ] || [ `find $path -type f | wc -l` -eq 0 ];then
#         echo "Error：Path does not exist or there is no file under the path"
#         exit 1
#     fi
# for i in `cat ${md5_path} | awk '\''{print $2}'\''`;do
#     if [[ ! -f $i ]];then
#         echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$i] \033[31m[MD5 check result：Removed]\033[0m" 2>&1
#         # 去除md5 从$md5_path
#         v2=${i//\//\\/}
#         sed -i "${v2}/d" ${md5_path}
#     fi
# done
# for list in `find $path -type f`;do
#     new_md5_arg1=`md5sum $list | awk '\''{print $1}'\''`
#     new_md5_arg2=`md5sum $list | awk '\''{print $2}'\''`
#     old_md5_arg2=`awk -v List="$list" '\''$2 == List{print $2}'\'' $md5_path`
#     if [[ "$new_md5_arg2" == "$old_md5_arg2" ]];then
#         old_md5_arg1=`awk -v List="$list" '\''$2 == List{print $1}'\'' $md5_path`
#         if [[ ! "$new_md5_arg1" == "$old_md5_arg1" ]];then
#             echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[31m[MD5 check result：Changed]\033[0m" 2>&1 
#             v3=${list//\//\\/}
#             sed -i "${v3}/d" ${md5_path}
#             md5sum ${list} >> ${md5_path}
#             if echo ${list} | grep -q "\.json"
#             then
#                 send_file "${list}"
#             fi
#         else
#             echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[32m[MD5 check result：Unchanged]\033[0m"
#         fi
#     else
#         new_file_md5=`md5sum ${list}`
#         md5sum ${list} >> ${md5_path}
#         echo -e "[Detection time：`date +"%Y-%m-%d %T.%N"`]  [File：$list] \033[31m[MD5 check result：Added]\033[0m" 2>&1 
#         if echo ${list} | grep -q "\.json"
#         then
#             send_file "${list}"
#         fi
#     fi
#     sleep 0.5
# done
# IFS=$SAVEIFS '