#!/bin/bash
export LANG=C

#Add registry cert.
export LC_ALL=C
export DOMAIN_NAME=registry.autowise.ai
openssl s_client -connect $DOMAIN_NAME:443 -showcerts </dev/null 2>/dev/null | openssl x509 -outform PEM | sudo tee /usr/local/share/ca-certificates/$DOMAIN_NAME.crt
sudo update-ca-certificates

echo "${HOME}/.coredump/core.%t.%e.%p" | sudo tee /proc/sys/kernel/core_pattern

mkdir -p $HOME/.coredump

docker run -ti --rm -e "TERM=xterm-256color" "$@" \
-e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" \
-v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ \
-v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
-w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
--network host --privileged \
-v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
registry.autowise.ai/awcar /bin/bash


# xterm -e bash -c 'docker run -ti --rm -e "TERM=xterm-256color" "$@" \
# -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" \
# -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ \
# -v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
# -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
# --network host --privileged \
# -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
# registry.autowise.ai/awcar /bin/bash' &

# sleep 5

# container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

# docker exec -it ${container} bash -c "export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
# /opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
# /home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:\
# /home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:\
# /usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:\
# /usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:\
# /usr/bin:/usr/sbin:/bin:/sbin; \
# #update_autowise_debs_and_models.sh; \

# cd autowise_test; cd log_based_simu; ls" 
