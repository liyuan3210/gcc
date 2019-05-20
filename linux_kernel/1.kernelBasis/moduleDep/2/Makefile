ifneq ($(KERNELRELEASE),)
#两个以上内核源文件 生成单独的内核模块名ma

#内核ma
obj-m :=ma.o
#下面的ma-objs前面必须和上面一样为ma
ma-objs := mains.o add.o

else

KDIR:= /lib/modules/2.6.18-53.el5/build

all:
		make -C $(KDIR) M=$(PWD) modules 
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order

endif
