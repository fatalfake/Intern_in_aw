#!/bin/bash
export LANG=C
export LC_ALL=C

docker pull registry.autowise.ai/awcar:latest

container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

if [[ ! -n ${container} ]]; 
then 
    xterm -e bash -c '
    docker run -ti --rm -e "TERM=xterm-256color" "$@" \
    -e "DISPLAY=localhost:10" -e "QT_X11_NO_MITSHM=1" \
    -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ \
    -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged \
    -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
    registry.autowise.ai/awcar:latest /bin/bash' &
fi

sleep 5

container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

sleep 5

docker exec -it ${container} bash -c "export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:/home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:/home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:/usr/bin:/usr/sbin:/bin:/sbin; update_autowise_debs_and_models.sh; cd qyc; ./monitor.sh" 

