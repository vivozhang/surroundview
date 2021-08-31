#!/bin/bash
sudo systemctl start tztek-jetson-service-mount-auto.service


path=/apollo/modules/surroundview
nohup ${path}/can2_can0_enable.sh > ${path}/enable_can.log &

