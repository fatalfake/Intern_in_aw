#!/bin/bash
export LANG=C
export LC_ALL=C

docker pull registry.autowise.ai/awcar:latest


container=`docker ps | grep regression_test | head -n 1 | cut -d ' ' -f1`

docker kill ${container}

cd /home/autowise/qyc/models

git pull http://git.autowise.ai/autowise/models

bash -c "
docker run --rm --name regression_test\
-v ~:/home/autowise/ \
-v /home/autowise/qyc/models/aw_models:/opt/ros/kinetic/share/aw_models \
-v /home/autowise/data/regression_test/:/home/autowise/data/regression_test/ \
-w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
--network host --privileged=true \
registry.autowise.ai/awcar:latest bash -c \"source /opt/ros/kinetic/setup.bash;\
source ~/.autowise/setup.sh;\
sudo apt-get install ros-kinetic-autowise-tools; \
cd /home/autowise/jenkins_ws/workspace/aw_autowise_test_branch/log_based_simu/; \
./install_dependencies.sh; \
export kRobotTypeHejia18T=hejia; \
python regression_parallelly.py --save --record\" "
if [ $? -ne 0 ];then
    echo "This means something failed"
    exit -1
fi

echo '=======================END=============================' 

# python regression_parallelly.py --save --record;\" "
