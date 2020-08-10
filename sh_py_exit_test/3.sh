#!/bin/bash

bash -c "echo python; python 2.py "


if [ $? -eq 255 ];then
    echo "This means something failed"
    exit -1
fi

