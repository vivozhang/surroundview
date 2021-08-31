#!/bin/bash
path=/apollo/modules/surroundview
#add libs
export LD_LIBRARY_PATH=${path}/opencv/lib/libopencv_local_lib.so:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

#delete mainboard.INFO
cd ${path}/openglwindow
rm -rf ./openglwindow.log.*

# sleep 3
cd ${path}/openglwindow
#nohup ./openglwindow > ${path}/runoob.log 2>&1
nohup ./openglwindow > /work/log/runoob.log &
#./openglwindow
