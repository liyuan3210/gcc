ifneq ($(KERNELRELEASE),)
obj-m := bus_base.o device_base.o driver_base.o
else
KDIR := /root/soft/rootfs/lib/modules/3.0.1/build
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order

endif
