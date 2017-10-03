#!/bin/bash

if [ $# \< 1 ]; then echo Usage: mku filename; exit; fi

# default partiton = 1
PARTITION=1
if [ $# \> 1 ]; then PARTITION=$2; fi

VDISK=vdisk
SECTOR=63
OFFSET=$(expr $SECTOR \* 512)

echo partition=$PARTITION sector=$SECTOR offset=$OFFSET
#read dummy

mount -o loop,offset=$OFFSET $VDISK /mnt

echo compiling $1.c .....
as86 -o u.o u.s
bcc -c -ansi $1.c

echo linking .......
ld86 u.o $1.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null 

echo copying $1 into $VDISK/bin/$1

cp -av a.out /mnt/bin/$1
umount /mnt

echo done $1
