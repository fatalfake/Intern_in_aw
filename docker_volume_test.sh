#!/bin/bash
export LANG=C

#Add registry cert.
export LC_ALL=C

docker pull registry.autowise.ai/awcar:latest

export display=`echo $DISPLAY`
container=`docker ps | grep registry.autowise.ai/awcar | head -n 1 | cut -d ' ' -f1`

if [[ ! -n ${container} ]]; 
then 
    xterm -e bash -c '
    docker run -ti --rm -e "TERM=xterm-256color" "$@" \
    -v /home/bag:/home/autowise/abc/bag \
    -e "DISPLAY=$display" \
    -v ~:/home/autowise/ \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged \
    registry.autowise.ai/awcar:latest /bin/bash' &
fi

sleep 5

