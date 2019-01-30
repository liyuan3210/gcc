#!/bin/bash
echo "			for语句"
#in后面跟上“sun mon tue wed thu fri sat”
for day in sun mon tue wed thu fri sat
do
	echo $day
done
echo ""

echo "			while语句"
num1=1
while [ 10 -gt $num1 ]
do
num1=$(($num1+1))
echo "value:"$num1	
done
echo ""

echo "			until与while相反，条件为假时才执行(下面的将是一个死循环)"
until [ 10 -gt $num1 ]
do
num1=$(($num1-1))
echo "value2:"$num1
done
echo ""

echo "			case语句(类似switch)"
#从键盘获取字符
read keypress
#判断
case "$keypress" in
	[A-Z] ) echo "uppercase latter";;
	[a-z] ) echo "lowercase latter";;
	[0-9] ) echo "digit";;
	* ) echo "other";;
esac
echo ""
