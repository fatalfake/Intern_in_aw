#!/bin/bash

bash -c "echo python; python 2.py "

exit=$?

echo "EXIT=$exit"

if [ ${exit} -eq 255 ];then
    echo "This means something failed"
    exit -1
fi
