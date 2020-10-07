obj-m += ken.o
#obj-$(CONFIG_KEN_TEST_HELLO)           += ken.o
all:
	make ARCH=arm CROSS_COMPILE=asdk-linux- -C /home/msinwu/rebuild/linux-kernel M=$(PWD) modules
	asdk-linux-gcc main.c -static
clean:
	make ARCH=arm CROSS_COMPILE=asdk-linux- -C /home/msinwu/rebuild/linux-kernel M=$(PWD) clean


