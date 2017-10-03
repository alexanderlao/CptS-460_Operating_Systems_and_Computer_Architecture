qemu-system-i386 -hda vdisk -m 520m -smp $1 -serial /dev/pts/0 -serial /dev/pts/2 \
-parallel /dev/pts/5 -localtime #-boot d
