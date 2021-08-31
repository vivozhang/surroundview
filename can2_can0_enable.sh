#!/bin/bash

test_flag=0

sudo /usr/lib/initramfs-tools/bin/busybox devmem 0x0c303000 32 0x0000c400
sudo /usr/lib/initramfs-tools/bin/busybox devmem 0x0c303008 32 0x0000c458
sudo /usr/lib/initramfs-tools/bin/busybox devmem 0x0c303010 32 0x0000c400
sudo /usr/lib/initramfs-tools/bin/busybox devmem 0x0c303018 32 0x0000c458

ifconfig | grep can0 > /dev/null 2>&1
if [ $? -ne 0 ]; then
	sudo ip link set can0 type can bitrate 500000 # dbitrate 125000 berr-reporting on fd on
	sudo ip link set up can0

	ifconfig | grep can0 > /dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo -e "\n\033[32msetup can0 done\033[0m"
	else
		echo -e "\n\033[31msetup can0 failed\033[0m"
		test_flag=1
	fi
else
	echo -e "\n\033[32msetup can0 done\033[0m"
fi


ifconfig | grep can2 > /dev/null 2>&1
if [ $? -ne 0 ]; then
	sudo ip link set can2 type can bitrate 500000 # triple-sampling on
 	sudo ip link set up can2

 	ifconfig | grep can2 > /dev/null 2>&1
 	if [ $? -eq 0 ]; then
 		echo -e "\033[32msetup can2 done\033[0m\n"
 	else
 		echo -e "\033[31msetup can2 failed\033[0m\n"
 		test_flag=1
 	fi
else
	echo -e "\n\033[32msetup can2 done\033[0m"
fi

if [ $test_flag -eq 1 ]; then
	exit 1
fi

# sudo ip link set can2 type can bitrate 500000 # triple-sampling on
# sudo ip link set up can2
# echo "setup can2 done\n------------------------------"
