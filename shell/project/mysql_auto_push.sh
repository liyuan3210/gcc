#!/bin/bash
basePath="/home/liyuan/soft/data"
#注意：文件符号不能为'_'或':',否则在windows下git克隆下载不了
time=$(date "+%Y-%m-%d_%H:%M:%S")
YY=$(date "+%Y")
MM=$(date "+%m")
DD=$(date "+%d")

echo "into call time:${time}">>"${basePath}/db/log.txt"

if [ -d "$basePath/db/$YY" ]
   then
       echo "目录已存在YY"
   else
       #echo "创建目录YY"
       mkdir "$basePath/db/$YY"
fi

if [ -d "$basePath/db/$YY/$MM" ]
   then
       echo "目录已存在MM"
   else
       #echo 创建目录MM"
       mkdir "$basePath/db/$YY/$MM"
fi

mysqldump --opt -d --databases pro -u adm -pxxx >"$basePath/db/$YY/$MM/table_$time.sql"
mysqldump -t pro -u adm -pxxx >"$basePath/db/$YY/$MM/data_$time.sql"

cd "$basePath"
git pull
git add *
git commit -m "Auto back data time:$time"
git push
