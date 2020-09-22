#!/bin/bash
export LANG=C

#Add registry cert.
export LC_ALL=C
# export DOMAIN_NAME=registry.autowise.ai
# openssl s_client -connect $DOMAIN_NAME:443 -showcerts </dev/null 2>/dev/null | openssl x509 -outform PEM | sudo tee /usr/local/share/ca-certificates/$DOMAIN_NAME.crt
# sudo update-ca-certificates

# echo "${HOME}/.coredump/core.%t.%e.%p" | sudo tee /proc/sys/kernel/core_pattern

# mkdir -p $HOME/.coredump
docker kill regression_test

docker pull registry.autowise.ai/awcar:latest

container=`docker ps | grep regression_test | head -n 1 | cut -d ' ' -f1`

# export display=`echo $DISPLAY`

if [[ ! -n ${container} ]]; 
then 
    docker run -ti --rm  --name regression_test \
    -v ~:/home/autowise/ \
    -v /home/autowise/qyc/models/aw_models:/media/autowise/d5d08454-3446-48d2-a747-58578bb723041/models \
    -w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
    --network host --privileged=true \
    registry.autowise.ai/awcar:latest /bin/bash
fi



