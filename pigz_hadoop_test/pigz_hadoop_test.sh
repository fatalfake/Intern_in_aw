#!/bin/bash

export HADOOP_HOME=/home/autowise/hadoop-3.1.1
export PATH=$HADOOP_HOME/bin:$PATH

hadoop version

file=/home/autowise/bags/regression_bags/2020_09_02_13_39_41_boschung_E1_regression_test.bag

time pigz -9 -p 1 -k ${file}

# hadoop fs -put ${file}.gz /bag_data/regression_bag