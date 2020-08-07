#!/bin/bash
export LANG=C
export LC_ALL=C

docker pull registry.autowise.ai/awcar:latest


container=`docker ps | grep regression_test | head -n 1 | cut -d ' ' -f1`

if [[ ! -n ${container} ]]; 
then 
    bash -c "
    docker run --rm --name regression_test\
    -v ~:/home/autowise/ \
    -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged=true \
    registry.autowise.ai/awcar:latest bash -c \"source /opt/ros/kinetic/setup.bash;\
    source ~/.autowise/setup.sh;\
    sudo apt-get install ros-kinetic-autowise-tools; \
    cd /home/autowise/jenkins/workspace/aw_autowise_test_branch/log_based_simu/; \
    ./install_dependencies.sh; \
    export kRobotTypeHejia18T=hejia; \
    python regression_parallelly.py --save --record\" "
    if [ $?==-1 ];then
        echo "This means something failed"
        exit -1
    fi
fi

echo '=======================END=============================' 

# python regression_parallelly.py --save --record;\" "
