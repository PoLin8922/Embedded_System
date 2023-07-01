# Embedded Operating System Lab
Every lab are done under /Linux folder. 
Here is the cmd to clone the folder.
```c=
git clone --depth=1 https://github.com/raspberrypi/linux
```
------------
## lab_02
1. Shrink your image size by deleting "Security" or "GPIO",careful that *LED shouldn't be closed.
2. For real-time patch, you need to find the correspond version on internet. 
-------------
## lab_03
1. All lab_03 files are run under Linux folder. (Provided by Rpi kernel)
2. Kernel module must make under the Linux file which you make the Rpi images, that is you need corresponding version with Rpi OS.

-------------
## lab_04
1. Makefile same with lab_03.


-------------
## lab_05
1. run 
```sh=
sh demo_lab5.sh
```
2. sl - display animations install
```c=
sudo apt install sl
```
3. check if there is any zombie process
```c=
ps aux | grep defunct | grep -v grep
```
