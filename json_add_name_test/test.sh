#!/bin/bash

list=./disney_all.yaml

park_id=`grep -E -o "park_id : !!str.+" ${list} | cut -d ' ' -f4`

echo ${park_id}

route_id=`grep -E -o "route_id : !!str.+" ${list} | cut -d ' ' -f4`
echo ${route_id}
jsonname=${park_id}_${route_id}

echo $jsonname

target_path=/home/autowise/qyc/json_add_name_test

if [ -f "${target_path}/fake.json" ]; then
    rm ${target_path}/fake.json
fi

jsonfile=`ls -t ${target_path} | grep ${jsonname} | head -n 1`

echo -e "\033[32m${jsonfile}\033[0m"

# sed -i "s/\"route_id\":\"[0-9]\+\"}/\"route_id\":\"${route_id}\",\"task_filename\":\"${jsonname}.json\"}/" ${jsonfile}