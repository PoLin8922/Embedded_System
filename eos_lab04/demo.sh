#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer Daniel &
./reader 192.168.0.55 8888 /dev/mydev
