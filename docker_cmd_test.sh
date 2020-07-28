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

container=`docker ps | grep registry.autowise.ai/awcar | head -n 1 | cut -d ' ' -f1`

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

container=`docker ps | grep registry.autowise.ai/awcar | head -n 1 | cut -d ' ' -f1`

docker exec -it ${container} bash -c "source /opt/ros/kinetic/setup.bash;\
source ~/.autowise/setup.sh;\
sudo apt-get install ros-kinetic-autowise-tools; \
cd autowise_test_new; cd log_based_simu; \
python regression_withlog.py; \

echo =======================END=============================;" 


# ./savecase.py -s ${start_sec} -e ${end_sec} ${bag_path} ${case_dir}; \
# ×××××××××××××××××××
# 问题：docker里面没有aw_bag的package，update了也没有
# ×××××××××××××××××××

# grep -q '${case_name}' ./config/regression_caselist && echo 'Case ${case_name} already exists in regression_caselist.' \
# || echo ${case_name} >> ./config/regression_caselist; \

# sort ./config/regression_caselist -o ./config/regression_caselist; \

# python regression.py; \
