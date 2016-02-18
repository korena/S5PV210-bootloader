#!/bin/bash
# for home
device="/dev/mmcblk0"
partition="/dev/mmcblk0p1"
# for work
#device="/dev/sdd"
#partition="/dev/sdd1"

#umount $partition 2> /dev/null
#echo "mkfs.vfat -F 32 $partition"
#mkfs.vfat -F 32 $partition
#echo "padding 16k ..."
#dd iflag=dsync oflag=dsync if=/dev/zero count=32 of=$device seek=1
echo "writing BL1 ..."
dd iflag=dsync oflag=dsync if=./target_bin/BL1.bin.boot of=$device seek=1
echo "writing bootloader ..."
dd iflag=dsync oflag=dsync if=./target_bin/BL2.bin.img of=$device seek=33
echo "writing kernel zImage ..."
#dd iflag=dsync oflag=dsync if=./kernel/images/active/zImage of=$device seek=160
