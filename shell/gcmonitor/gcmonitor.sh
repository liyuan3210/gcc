if [ $# != 3 ] ; then 
echo "USAGE: $0 ¡¾pid¡¿ ¡¾interval¡¿ ¡¾times¡¿" 
echo " e.g.: $0 pidnumber 3 1000 " 


exit 1; 
fi 
count=0
interval=$2
pid=$1

while [ $3 -gt $count ]
do
echo "`date +%Y%m%d%H%M%S`------- `jstat -gcutil $pid | grep -v FGC`" | tee -a gcmon.log
count=`expr $count + 1`
sleep $interval
done
