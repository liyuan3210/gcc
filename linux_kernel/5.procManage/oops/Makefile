ifneq ($(KERNELRELEASE),)

obj-m := oops.o

else
	
KDIR := ../5-3-1/linux-2.6.36
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order

endif
