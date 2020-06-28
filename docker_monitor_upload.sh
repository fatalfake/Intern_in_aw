#!/bin/bash

export LANG=C

# Add registry cert.
export LC_ALL=C
export DOMAIN_NAME=registry.autowise.ai
openssl s_client -connect $DOMAIN_NAME:443 -showcerts </dev/null 2>/dev/null | openssl x509 -outform PEM | sudo tee /usr/local/share/ca-certificates/$DOMAIN_NAME.crt
sudo update-ca-certificates

sudo systemctl restart docker

echo "${HOME}/.coredump/core.%t.%e.%p" | sudo tee /proc/sys/kernel/core_pattern

mkdir -p $HOME/.coredump

# docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined --network host --privileged -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar /bin/bash

# docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -w /home/autowise/ -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar /bin/bash

docker run -ti --rm -e "TERM=xterm-256color" "$@" -e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" -v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined --network host --privileged -v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models registry.autowise.ai/awcar bash -c 'cd qyc; ./monitor.sh'

# sleep 5

# container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

# # docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1
# # echo ${container}

# #TO-DO:在run docker的时候把环境变量设置好?
# docker exec -it ${container} bash -c "cd qyc; source ~/.autowise/setup.sh; ./monitor.sh"