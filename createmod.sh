insmod ken.ko
mknod /dev/ken c 242 0
chmod 755 /dev/ken
chmod 755 a.out
ls -al /dev/ken
