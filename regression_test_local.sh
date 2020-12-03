#!/bin/bash
export LANG=C
export LC_ALL=C

docker pull registry.autowise.ai/awcar:latest

regressionpath=/home/autowise/autowise_test_new/log_based_simu

container=`docker ps | grep regression_test | head -n 1 | cut -d ' ' -f1`

cd /media/autowise/d5d08454-3446-48d2-a747-58578bb723041/models

git pull http://git.autowise.ai/autowise/models

if [[ ! -n ${container} ]]; 
then 
    bash -c "
    docker run --rm --name regression_test\
    -v ~:/home/autowise/ \
    -v /media/autowise/d5d08454-3446-48d2-a747-58578bb723041/models:/opt/ros/kinetic/share/aw_models \
    -v /home/autowise/data/regression_test/:/home/autowise/data/regression_test/ \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged=true \
    registry.autowise.ai/awcar:latest bash -c \"source /opt/ros/kinetic/setup.bash;\
    source ~/.autowise/setup.sh;\
    sudo apt-get install ros-kinetic-autowise-tools; \
    cd ${regressionpath}; \
    ./install_dependencies.sh; \
    export kRobotTypeHejia18T=hejia; \
    python regression_parallelly.py;\" "
    if [ $? -ne 0 ];then
        echo "This means something failed"
        exit -1
    fi
fi

echo '=======================END=============================' 

# python regression_parallelly.py --save --record;\" "
