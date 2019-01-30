#!/bin/bash
#定时写入文件test.txt包含写入时间(定时需要配置crontab)
time=$(date "+%Y-%m-%d %H:%M:%S")
echo "hello call time : ${time}">>/home/liyuan/soft/test.txt