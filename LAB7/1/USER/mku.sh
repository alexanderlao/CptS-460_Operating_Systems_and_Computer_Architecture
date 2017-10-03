VFD=../mtximage

mount -o loop $VFD /mnt
rm /mnt/bin/*
umount /mnt

echo --------------------- make $1 -----------------------
bcc -c -ansi $1.c
as86 -o u.o u.s
ld86 -o $1 u.o $1.o ../mtxlib /usr/lib/bcc/libc.a

echo --------------------- make $2 -----------------------
bcc -c -ansi $2.c
as86 -o u.o u.s
ld86 -o $2 u.o $2.o ../mtxlib /usr/lib/bcc/libc.a

mount -o loop $VFD /mnt
cp $1 /mnt/bin/$1
cp $2 /mnt/bin/$2
ls /mnt/bin
umount /mnt

rm *.o $1
rm *.o $2
echo done $1 $2