#!/bin/bash

docker kill regression_test

export HADOOP_HOME=/home/autowise/hadoop-3.1.1
export PATH=$HADOOP_HOME/bin:$PATH

hadoop version


# hadoop fs -put ${file}.gz /bag_data/regression_bag

# find . -name "LC*" | xargs tar cvf - | pigz -9 -p 1 -k > test.tar.gz
