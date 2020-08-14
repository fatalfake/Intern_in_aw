import subprocess

str1 = ".0.0:"

cmd = "netstat -lntp | grep rviz | grep 0.0%s" %str1

s = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)

p = s.stdout.read()

print len(p)
print p