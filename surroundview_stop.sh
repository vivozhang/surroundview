#!/bin/bash
input1=openglwindow

PID=$(ps -e|grep $input1|awk '{printf $1}')

if [ $? -eq 0 ]; then
    echo "process id:$PID"
else
    echo "process $input1 not exit"
    exit
fi


kill -9 ${PID}

if [ $? -eq 0 ];then
    echo "kill $input1 success"
else
    echo "kill $input1 fail"
fi
