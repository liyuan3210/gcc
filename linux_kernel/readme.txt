			linux内核编程
主要内容
	1.kernelBasis
	内核模块编译，内核模块开发

	2.uboot
	uboot基础，uboot编译，uboot命令

	3.linuxSysBuild
	嵌入式内核制作，根文件系统制作，嵌入式文件系统

	4.memManage
	内存管理，进程地址空间，内核地址空间，linux内核链表，linux内核定时器

	5.procManage
	linux进程控制，Linux进程调度，Linux系统调用实现，proc文件系统，内核异常分析

	6.charDriver
	LINUX驱动程序介绍，字符设备驱动程序设计，虚拟文件系统VFS，竞争与互斥(驱动调试技术)

	7.highChar
	Ioctl设备控制，内核等待队列/阻塞型字符设备驱动，poll设备方法，自动创建设备文件

	8.hdOperation
	mmap设备方法，硬件访问，混杂设备驱动
	
	croesr
	驱动深入

-----------------------------------
make时错误:
make: 警告：检测到时钟错误。您的创建可能是不完整的
原因：如果上一次编译时为20060101,你现在再编译就会报这样的错误.
解决：把时间改了或运行下来命令再make
find . -type f -exec touch {} \;

