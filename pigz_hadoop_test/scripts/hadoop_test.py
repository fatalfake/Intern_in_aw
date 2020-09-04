import subprocess

cmd = "export HADOOP_HOME=/home/autowise/hadoop-3.1.1;export PATH=$HADOOP_HOME/bin:$PATH;hadoop version"

s1 = subprocess.Popen(cmd, shell=True)

s1.wait()

print "\033[32mCompressing... \033[0m"

cmd1 = "read -r -a arr <<< `hadoop fs -ls /bag_data/regression_bag | awk '{print $8}' | awk -F/ '{print $4}'`; echo ${arr[-1]}"


s2 = subprocess.Popen(cmd1, executable='/bin/bash', stdout=subprocess.PIPE, shell=True)

ppp = s2.stdout.read().strip()

print 'ppp is %s ppp end' %ppp

cmd2 = "hadoop fs -ls /bag_data/regression_bag/1210"

s3 = subprocess.Popen(cmd2, shell=True)

s3.communicate()

s3code = s3.returncode

print s3code

cmd3 = "cd /home/autowise/qyc/pigz_hadoop_test; find . -name \"LC*\" | xargs tar cvf - | pigz -9 -p 1 -k > test.tar.gz"

s4 = subprocess.Popen(cmd3, shell=True)

s4.communicate()