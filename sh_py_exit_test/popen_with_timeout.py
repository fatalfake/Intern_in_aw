import time
import subprocess

print time.time()

command = "echo $PYTHONPATH; sleep 10"
s = subprocess.Popen(command, shell=True)
s.communicate()