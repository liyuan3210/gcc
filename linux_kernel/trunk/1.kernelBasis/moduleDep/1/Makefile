#第一次执行KERNELRELEASE是空的,所以执行else里面的
ifneq ($(KERNELRELEASE),)

obj-m :=hello.o

#else块
else

KDIR:= /lib/modules/2.6.18-53.el5/build

all:
#KDIR    依赖内核模块源代码路径(内核编译安装路径)
#PWD     表示内核代码在哪(当前目录)
#modules 编译的是模块
	make -C $(KDIR) M=$(PWD) modules 

clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order

endif