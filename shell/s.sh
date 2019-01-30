#!/bin/bash
echo "			基础语法"
echo "hello world!"
str="str string value!"
num=2
echo "str is:"$str
echo "the ${num}is num "
echo ""

echo "			默认变量"
#$#：传入脚本命令行参数个数
echo "$# value is:"$#
#$*：所有命令行参数值
echo "$* value is:"$*
#$0：命令本身(shell文件名)
echo "$0 value is:"$0
#$1:第一个参数
echo "$1 value is:"$1
#$2:第二个参数
echo "$2 value is:"$2
echo ""

echo "			局部变量(自定义方法执行)"
echo "局部变量"
lv="var1"
echo "once is："$lv
echo ""
#定义方法
function func1
{
#传参数
	echo "on func1 arg1 is:"$1
	echo "on func1 arg2 is:"$2
	local lv="var2"
	echo "on func1 is:"$lv
}
endvalue=$(func1 2 3)
#打印,并执行方法func1
echo " end value is:"$endvalue
echo ""
echo "on outside is:"$lv#观察其lv值没经过方法func1而改变
echo ""

echo "			if语句(可以参考下面的'比较符号')"
num1=3
num2=2
if [ $num1 -eq $num2 ]
then
	echo "num1 and num2 equal."
elif [ $num1 -gt $num2 ]
then
	echo "num1 > num2!"
elif [ $num1 -ne $num2 ]
then
	echo "other!"
fi
echo ""

echo "			file判断(可以参考下面的'文件判断')"
folder=/home
# && 表示左边条件为真时才执行右边的echo语句
[ -r "$folder" ] && echo "can read $folder"
# || 表示左边条件为假时才执行右边的echo语句
[ -f "$folder" ] || echo "this is not file"
echo ""

	#需要注意的
#变量赋值时“=”两边都不能够有空格
#BASH中的语句不许要分号
#后面的if,在"["和"]"符号的左右都留有空格

	#比较符号#
#相等 -eq = 如 if [ $a = $b ](也可以用eq)
#不等 -ne != 如 if [ $a -ne $b ]
#大于 -gt > 如 if [ $a -gt $b ]
#小于 -lt < 如 if [ $a -lt $b ]
#大于或等于 -ge 如 if [ $a -ge $b ]
#小于或等于 -le 如 if [ $a -le $b ]
#为空 -z 如 if [ -z $a ]
#不为空 -n 如 if [ -n $a ]

	#文件判断#
#-e 文件已经存在
#-f 是普通文件
#-s 文件大小不为零
#-d 文件是一个目录
#-r 文件对当前用户可以读取
#-w 文件对当前用户可以写入
#-x 文件对当前用户可以执行