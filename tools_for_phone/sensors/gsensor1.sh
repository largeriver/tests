#!/bin/sh

echo start
while true 
do
	dd if=/dev/mma7660fc_dev of=/data/gs ibs=40 count=1 >/dev/null 2>&1
	accs=`cat /data/gs`
	acc_msg=`expr "$accs" : '\([a-z]*\)'`
	if [ $acc_msg = "acceleration" ]; then
		acc_x=`expr "$accs" : 'acceleration:\([0-9]*\)'`
		acc_y=`expr "$accs" : 'acceleration:[0-9]*:\([0-9]*\)'`
		acc_z=`expr "$accs" : 'acceleration:[0-9]*:[0-9]*:\([0-9]*\)'`

		acc_x=$(((acc_x - 981000)/1000))
		acc_y=$(((acc_y - 981000)/1000))
		acc_z=$(((acc_z - 981000)/1000))

		
		printf "% 6d\t % 6d\t % 6d\n" $acc_x $acc_y $acc_z
		
	fi
	sleep 1
done

