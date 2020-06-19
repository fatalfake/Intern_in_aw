#!/bin/bash
export LANG=C
last_event=' '
inotifywait -mrq --event 'attrib' --exclude '(.*/*\.log|.*/*\.swp|.*/*\.svn|.*/*\.swx|4913)' /opt/ros/kinetic/share/aw_launch/config/conf/tasks | while read event
do
    if [ "${event}" = "${last_event}" ];then
        last_event=' '
        continue
    fi
    last_event=${event}
    filepath=`echo ${event} | cut -d ' ' -f 1 | sed 's/\.//'`
    filename=`echo ${event} | cut -d ' ' -f 3`
    # filename=`echo ${filename_meta} | cut -d . -f 1-2`
    # echo ${filepath}
    # echo ${filename_meta}
    # echo ${filename}
    if echo ${filename} | grep -q '\.yaml' 
    then
        # echo ${event}
        path_name=${filepath}${filename}
        export PLANNING_TASK=${path_name}
        echo 'Task file is: '${PLANNING_TASK}
        project_name=`echo ${filepath} | cut -d / -f 10`
        echo 'Project name is: '${project_name}
        rosrun aw_launch aw_config.py --cfg buss2 ${project_name}
        gnome-terminal -x bash -c "source ~/.autowise/setup.sh;roslaunch aw_hdmap hdmap_runtime_env.launch"
        sleep 40
        source ./devel/setup.sh
        roslaunch aw_global_planning route_points_generator.launch
        echo 'Task complete.'
    fi
    # echo '***********'
done
