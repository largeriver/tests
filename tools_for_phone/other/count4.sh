max=$1
count=0
while [ $max -gt 0 ];do
  count=$(($count+$max/2))
  max=$(($max/2))
done
echo $count
echo

