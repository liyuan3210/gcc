#!/bin/bash
#数据文件log.txt定时提交到gitee包含写入的时间(定时需要配置crontab)
time=$(date "+%Y-%m-%d %H:%M:%S")
echo "call push time : ${time}">>/home/liyuan/tool/prv/log.txt
cd /home/liyuan/tool/prv
git pull
git add *
git commit -m "sys call time:${time}"
git push