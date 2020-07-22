#!/bin/bash
export LANG=C
export LC_ALL=C

docker pull registry.autowise.ai/awcar:latest

xterm -e bash -c '
    docker run -ti --rm -e "TERM=xterm-256color" "$@" \
    -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" \
    -e "PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
    /opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
    /home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:\
    /home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:\
    /usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:\
    /usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:\
    /usr/bin:/usr/sbin:/bin:/sbin;" \
    -e "PYTHONPATH=$PYTHONPATH:/opt/ros/kinetic/bin:/opt/ros/kinetic/share:/usr/local/bin:/usr/local/lib/python2.7/dist-packages;" \
    -e "ROS_ROOT=/opt/ros/kinetic/share/ros; " \
    -e " LD_LIBRARY_PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib:\
    /opt/ros/kinetic/lib:/opt/ros/kinetic/lib/x86_64-linux-gnu:/opt/ros/kinetic/share:\
    /opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/lib; " \
    -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ \
    -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged \
    -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
    registry.autowise.ai/awcar:latest /bin/bash' &

sleep 5

container=`docker ps | grep registry.autowise.ai/awcar | head -n 1 | cut -d ' ' -f1`

docker exec -it ${container} bash -c " \
source /opt/ros/kinetic/setup.bash; \
update_autowise_debs_and_models.sh; \
sudo apt-get install ros-kinetic-autowise-tools; \
source ~/.autowise/setup.sh; \
cd autowise_test_newer; cd log_based_simu; \
sort ./config/regression_caselist -o ./config/regression_caselist; \
python regression.py; \

echo =======================END=============================;" 