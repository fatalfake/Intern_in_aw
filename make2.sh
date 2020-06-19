filename=$1
size=`ls -l $filename| awk '{print $5}'`

echo $filename
echo $size
