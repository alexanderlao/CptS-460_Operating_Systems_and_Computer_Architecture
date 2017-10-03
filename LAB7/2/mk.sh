as86 -o ts.o ts.s
bcc  -c -ansi t.c
ld86 -d -o mtx0 ts.o t.o OBJ/*.o mtxlib /usr/lib/bcc/libc.a

mount -o loop mtximage /mnt
cp mtx0 /mnt/boot/mtx0
umount /mnt
rm *.o mtx0

echo done
