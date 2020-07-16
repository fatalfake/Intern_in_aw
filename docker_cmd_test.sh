#!/bin/bash
export LANG=C

#Add registry cert.
export LC_ALL=C
# export DOMAIN_NAME=registry.autowise.ai
# openssl s_client -connect $DOMAIN_NAME:443 -showcerts </dev/null 2>/dev/null | openssl x509 -outform PEM | sudo tee /usr/local/share/ca-certificates/$DOMAIN_NAME.crt
# sudo update-ca-certificates

# echo "${HOME}/.coredump/core.%t.%e.%p" | sudo tee /proc/sys/kernel/core_pattern

# mkdir -p $HOME/.coredump

# # echo '==================Reading parameters================'
# bag_path=$1
# # echo "----------------${bag_path}"
# start_sec=$2
# # echo "----------------${start_sec}"
# end_sec=$3
# # echo "----------------${end_sec}"
# case_dir=$4
# # echo "----------------${case_dir}"
# case_name=`awk -F/ '{print $NF}'<<<"${case_dir}"`
# echo ${case_name}
# # echo '===================================================='

docker pull registry.autowise.ai/awcar:latest

container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

if [[ ! -n ${container} ]]; 
then 
    xterm -e bash -c '
    docker run -ti --rm -e "TERM=xterm-256color" "$@" \
    -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" \
    -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ \
    -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged \
    -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
    registry.autowise.ai/awcar:latest /bin/bash' &
fi

sleep 5

container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

docker exec -it ${container} bash -c "export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
/opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
/home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:\
/home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:\
/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:\
/usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:\
/usr/bin:/usr/sbin:/bin:/sbin; \
export PYTHONPATH=$PYTHONPATH:/opt/ros/kinetic/bin/:/usr/local/bin:/usr/local/lib/python2.7/dist-packages; \
export ROS_ROOT=/opt/ros/kinetic/share/ros; \
export ROS_MASTER_URI=http://localhost:11311; \
export ROS_PYTHON_VERSION=2; \
export ROS_VERSION=1; \
export ROS_ETC_DIR=/opt/ros/kinetic/etc/ros; \
export ROS_DISTRO=kinetic; \
export LD_LIBRARY_PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib:\
/opt/ros/kinetic/lib:\
/opt/ros/kinetic/lib/x86_64-linux-gnu:\
/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib; \
source /opt/ros/kinetic/setup.bash; \
source ~/.autowise/setup.sh; \
cd autowise_test_new; cd log_based_simu; \
sort ./config/regression_caselist -o ./config/regression_caselist; \
python regression.py; \

echo =======================END=============================;" 


# ./savecase.py -s ${start_sec} -e ${end_sec} ${bag_path} ${case_dir}; \
# ×××××××××××××××××××
# 问题：docker里面没有aw_bag的package，update了也没有
# ×××××××××××××××××××

# grep -q '${case_name}' ./config/regression_caselist && echo 'Case ${case_name} already exists in regression_caselist.' \
# || echo ${case_name} >> ./config/regression_caselist; \


# python regression.py; \
