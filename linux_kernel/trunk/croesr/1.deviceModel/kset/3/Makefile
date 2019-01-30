ifneq ($(KERNELRELEASE),)
obj-m := my_kset.o
else
KDIR := /root/soft/rootfs/lib/modules/3.0.1/build
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order

endif
