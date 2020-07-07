#!/bin/bash
export LANG=C

#Add registry cert.
export LC_ALL=C
export DOMAIN_NAME=registry.autowise.ai
openssl s_client -connect $DOMAIN_NAME:443 -showcerts </dev/null 2>/dev/null | openssl x509 -outform PEM | sudo tee /usr/local/share/ca-certificates/$DOMAIN_NAME.crt
sudo update-ca-certificates

sudo systemctl restart docker

echo "${HOME}/.coredump/core.%t.%e.%p" | sudo tee /proc/sys/kernel/core_pattern

mkdir -p $HOME/.coredump

echo '==================Reading parameters================'
bag_name=$1
echo "----------------${bag_name}"
start_sec=$2
echo "----------------${start_sec}"
end_sec=$3
echo "----------------${end_sec}"
case_dir=$4
echo "----------------${case_dir}"
echo '===================================================='


xterm -e bash -c 'docker run -ti --rm -e "TERM=xterm-256color" "$@" \
-e "DISPLAY=:0" -e "QT_X11_NO_MITSHM=1" \
-v /tmp/.X11-unix:/tmp/.X11-unix -v ~:/home/autowise/ \
-v /opt/ros/kinetic/share/aw_global_planning/launch:/opt/ros/kinetic/share/aw_global_planning/launch \
-w /home/autowise/ --ulimit core=-1 --security-opt seccomp=unconfined \
--network host --privileged \
-v /opt/ros/kinetic/share/aw_models:/opt/ros/kinetic/share/aw_models \
registry.autowise.ai/awcar /bin/bash' &

sleep 5

container=`docker ps | grep registry.autowise.ai/awcar | cut -d ' ' -f1`

docker exec -it ${container} bash -c "export PATH=/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
/opt/ros/kinetic/bin:/opt/ros/kinetic/share/euslisp/jskeus/eus//Linux64/bin:\
/home/autowise/hadoop-3.1.1/bin:/home/autowise/hadoop-3.1.1/sbin:\
/home/autowise/bin:/home/autowise/.local/bin:/usr/local/sbin:\
/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:\
/usr/local/games:/snap/bin:/usr/local/sbin:/sur/local/bin:\
/usr/bin:/usr/sbin:/bin:/sbin; \
export PYTHONPATH=$PYTHONPATH:/opt/ros/kinetic/bin/:/usr/local/bin:/usr/local/lib/python2.7/dist-packages; \
pip install sqlalchemy;
export ROS_ROOT=/opt/ros/kinetic/share/ros; \
export ROS_MASTER_URI=http://localhost:11311; \
export ROS_PYTHON_VERSION=2; \
export ROS_VERSION=1; \
export ROS_ETC_DIR=/opt/ros/kinetic/etc/ros; \
export ROS_DISTRO=kinetic; \
source ~/.autowise/setup.sh; \
source /opt/ros/kinetic/setup.bash; \
cd autowise_test; cd log_based_simu; python regression.py;
echo =======================END=============================;" 