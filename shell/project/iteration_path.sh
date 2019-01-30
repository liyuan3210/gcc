#!/bin/bash
#文件夹及文件迭代功能
basePath="/home/liyuan/soft/test"

function fileIter()
{
	echo "into $1/$2"
	#文件与文件夹判断
	for f2 in $(ls $1/$2)
	do
		if [ -d "$1/$2/$f2" ]
		  then
			  echo "子目录:$f2"
		   fileIter $1/$2 $f2
		elif [ -f "$1/$2/$f2" ]
		 then
		  echo "子文件:$f2"
		else
		  echo "undefined"
		fi
	done
}

for f in $(ls $basePath)
do

	#文件与文件夹判断
	if [ -d "$basePath/$f" ]
	then
	 echo "目录:$f"
	  fileIter $basePath $f
	   #echo "子目录里面文件1$res"
	elif [ -f "$basePath/$f" ]
	 then
	 echo "文件:$f"
	else 
	 echo "undefined"
	fi

	#相等判断(测试)
	if [ "test2.sh" == "$f" ]
	then
	echo "相等判断$f"
	fi

done