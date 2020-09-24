import subprocess
import re

check_port_cmd = "netstat -lnp | grep 127.0.0.1:"
check_port_process = subprocess.Popen(check_port_cmd, stdout=subprocess.PIPE, shell=True)
check_port_info = check_port_process.stdout.read()
check_port_info_lines = check_port_info.split('\n')
check_port_info_lines.remove(check_port_info_lines[-1])

forbid_port_list = []

for i in check_port_info_lines:
    j = re.split(r"[ ]+", i)
    l = j[3].split(':')[1]
    forbid_port_list.append(l)
    k = j[4].split(':')[1]
    if k != '*':
        forbid_port_list.append(k)

port = "631"

forbid_port_list = list(set(forbid_port_list))
forbid_port_list.sort()

print forbid_port_list

if port in forbid_port_list:
    while True:
        port = str(int(port) + 1)
        if port not in forbid_port_list:
            break

print port