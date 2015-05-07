echo start
while [ 1 ] ; do
dd if=/dev/mma7660fc_dev of=/data/gs ibs=40 count=1 >/dev/null 2>&1
cat /data/gs
echo ""
sleep 1
done

