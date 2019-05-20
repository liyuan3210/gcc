			一.LINUX驱动程序介绍
知识结构：
	·linux驱动程序设计模式(40%)
	·内核相关知识(30%)
	·硬件相关知识(30%) 
驱动分类：
	·字符设备驱动(重点)
	是一种"最小按字节访问"的设备，字符驱动则负责驱动字符设备，这样的驱动通常实现open,close,read,write系统调用.
	·网络接口驱动(重点)
	任何网络事物都通过一个接口来进行,一个接口通常是一个硬件设备(eth0),但是它也可以使一个纯粹的软件设备，比如回环接口(Io)，一个网络接口负责发送和接收数据报文。
	·块设备驱动
	1)在大部分的unix系统中，块设备不能够按字节处理数据，只能一次传送一个或多个长度是512字节(或更大的2次幂的数)的整块数据。
	2)而linux则允许块设备传送任意数目字节，因此块和字符设备的区别仅仅是"驱动的与内核的接口不同".
驱动程序安装：
	·模块方式(前面insmod方式已知)
	·直接编译进内核(方法与步骤如下)
	1)在~/linux-version/drivers/char目录下编写hello.c内核模块代码（与模块方式方式相同）.
	2)修改~/linux-version/drivers/char/Kconfig(可以用make menuconfig在Device drivers>Character devices下看到hello driver(NEW) ),添加内容如下：
	config HELLO
	       bool "hello driver"
	3)修改~/linux-version/drivers/char/Makefile,添加内容如下：
	obj-$(CONFIG_HELLO)      += hello.o
	4)最后编译内核.
驱动程序的使用：qdcxsy.jpg
	linux中设备文件在/dev目录中，linux用户程序通过设备文件(又名:设备节点)来使用驱动程序操作"字符设备"和"块设备".
	在dev中使用ls -l可以看到第一列第一个字母，用户是通过字符设备文件来找到字符设备驱动的。
	c打头:字符设备
	b打头:块设备
	d打头:表示是目录
-----------------------------------------------------------------------------
			二.字符设备驱动程序设计
知识点:
	·设备号
	·创建设备文件
	·设备注册
	·重要数据结构
	·设备操作
设备号:
	·字符设备通过字符设备文件来存储，在/dev目录下通过ls -l输出的第一行第一个字符为c的中会看到字符设备文件有两个数字，由一个","号隔开。这分别是设备文件的主次设备编号。
	主设备号作用：用来建立"字符设备文件"与"字符驱动程序"对应关系的(映射设备类型).
	次设备号作用：次设备号被驱动程序用来辨别操作的是哪个设备(区分同类型的设备).
	·内核中设备号类型
	dev_t类型:其实质是unsigned int 32位整型，其中高12位是主设备号，低20位是次设备号
	内核中分解主设备号:MAJOR(dev_t dev)
	内核中分解次设备号:MINOR(dev_t dev)
	·linux分配设备号
	1)静态申请:确定系统没有使用的设备号，使用register_chrdev_region函数注册设备(可能会发生设备号冲突，申请不成功)。
	int register_chrdev_region(dev_t from,unsigned count,const char *name)
	from:希望申请使用的设备号
	count:希望申请使用设备号数目
	name:设备名
	注意：主设备号不变，次设备号增加，体现在/proc/devices.
	2)动态分配:内核帮助分配一个不会冲突的号,使用alloc_chrdev_region函数注册.
	int alloc_chrdev_region(dev_t *dev,unsigned baseminor,unsigned count,const char *name)
	dev:用它来填值，获取内核分配到的设备号
	baseminor:起始次设备号
	count:希望申请使用设备号数目
	name:设备名
	体现在cat /proc/devices.
	·注销设备号
	不管使用哪一种方法分配设备号，在不使用它时应该把它释放注销掉。
	void unregister_chrdev_region(dev_t from,unsigned count)，释放从from开始的count个设备号。
	from:设备号
	count:要释放的设备个数
创建设备文件：
	·使用mknod命令手工创建
	用法：mknod filename type najor minor
	filename:设备文件名
	type:设备文件类型
	major:主设备号
	minor:次设备号
	例：mknod serial0 c 100 0
	·自动创建
	在代码里面通过函数自动的创建.代码见 highChar/5-7-4
重要数据结构:
	·struct file:系统每打开一个文件，内核空间都有一个关联的file，它由内核在打开文件时创建，在文件关闭后释放.
	loff_t f_pos /*文件读写位置*/
	struct file_operations *f_op
	·struct inode:用来记录文件的物理上的信息，所以与file不同，但只有一个inode结构。
	dev_t i_rdev:设备号
	·struct file_operations:一个函数指针的集合，定义能在设备上进行的操作。结构中的成员指向驱动中的函数，这个函数实现一个特别的操作，对不支持的操作保留位NULL。
	struct file_operations mem_fops={
		.owner=THIS_MODULE,//谁拥有这个结构(模块引用技术,被其它模块使用自动加1，用完后自动减1，卸载模块时会进行判断)
		.llseek=mem_seek,//函数指针(驱动开发大部分的工作)
		.read=mem_read,//函数指针(驱动开发大部分的工作)
		.write=mem_write,//函数指针(驱动开发大部分的工作)
		.ioctl=mem_ioctl,//函数指针(驱动开发大部分的工作)
		.open=mem_open,//函数指针(驱动开发大部分的工作)
		.release=mem_release,//函数指针(驱动开发大部分的工作)
	};
	模块引用技术：见 mkyyjs.jpg(输入lsmod命令查看)
	用来防止模块被使用时被卸载掉。
设备注册：
	linux2.6中使用struct cdev来描述。
	字符设备注册可分为3个步骤：
	·分配cdev
	struct cdev *cdev_alloc(void)
	·初始化cdev
	void cdev_init(struct cdev *cdev,const struct file_operations *fops)
	cdev:待初始化的cdev结构
	fops:设备对应的操作函数
	·添加cdev
	int cdev_add(struct cdev *p,dev_t dev,unsigned count)
	p:待添加到内核的字符设备结构
	dev:设备号
	count:添加的设备个数
最重要的是要实现file_operations里面的函数:
	·int (*open)(struct inode *,struct file *) //需完成：初始化设备，标明次设备号
	在设备文件上的第一个操作，并不要求驱动程序一定要实现这个方法。如果该项为NULL，设备的打开操作永远成功。
	·void (*release)(struct inode *,struct file*)//需完成：关闭设备
	当设备文件被关闭时调用这个操作。与open相反，release也可以没有.
	·读写方法
	·ssize_t xxx_read(*read)(struct file *filp,char __user *buff,size_t size,loff_t *offp)
	·ssize_t xxx_write(*write)(struct file *filp,char __user *buff,size_t size,loff_t *offp)
	//读和写都完成类似的工作，从设备中读取数据到用户空间；将数据传递给驱动程序，它们的原型也相似
	//两个方法,filp是文件指针，count是请求传输的数据量。buff指向数据缓存，offp当前的访问位置，size要求读写大小
	·unsigned int (*poll)(struct file*,struct poll_table_struct *)
	对应select系统调用
	·int (*ioctl)(struct inode *,struct file*,unsigned int,unsigned long)
	控制设备
	·int (*mmap)(struct file*,struct vm_area_struct *)
	将设备映射到进程虚拟地址空间中
	·off_t (*llseek)(struct file *filp,loff_t offset,int whence)
	修改文件的当前读写位置，并将新位置作为返回地址,参数offset为根据参数whence来移动读写位置的位移数，参数whence为下列其中一种：
	SEEK_SET             参数offset即为新的读写位置
	SEEK_CUR             当前读写位置后增加offset个位移量。
	SEEK_END             将读写位置指向文件尾后再增加offset个位移量
	·int cdev_del(struct cdev *p)
	字符设备的注销,p(要注销的字符设备结构)
专门用于访问用户空间的指针函数:
	/*对于两个方法，filp是文件指针，count是请求传输的数据量，buff参数指向数据缓存,offp指向最后访问的位置*/
	注意：buff参数是用户空间指针，它不能够被内核代码直接引用因为 用户空间指针在内核空间时可能根本是无效的，
	的，没有那个地址的映射.
	·int copy_from_user(void *to,const void __user *from,int n)//write时会用到
	·int copy_to_user(void __user *to,const void *from,int n)//read时会用到
数据模型：sjmx.jpg
设备操作/实例分析：5-6-1
	查询设备号：cat /proc/devices
	实例运行测试：
	1.先编译好驱动程序，应用测试程序
	2.运行驱动程序与 mknod memdev0 c 251 0 命令
	3.运行应用程序
-----------------------------------------------------------------------------
			三.虚拟文件系统VFS
描述：
文件类型：
	.普通文件
	.目录文件
	.链接文件
	.设备文件
	.socket文件
	.管道文件
文件系统类型：
	.磁盘文件系统(如ext2)
	.flash文件系统(如yaffs2)
	.网络文件系统(如NFS)
	.特殊文件系统(如/sys)
文件访问:wjfw.jpg
虚拟文件系统:xnwjxt0.jpg,xnwjxt1.jpg
-----------------------------------------------------------------------------
			四.竞争与互斥(驱动调试技术)
驱动调试：
	常见调试方法：
	·打印调试(应用中使用printf,内核中使用printk)
	·调试器调试(kgdb)
	·查询调试(proc文件系统)
合理使用pintk,定义define PDEBUG变量,编译时加上-DPDEBUG

并发控制：
	并发(多个执行单元同时被执行)，竞态(并发的执行单元对共享资源的访问导致的竞争状态<硬件上的资源与软件上的全局变量>)
	为了保证上面出现的问题，处理并发的常用技术是加锁或者互斥，即保证子任何时间只有一个执行单元可以操作共享资源。
	一个完整linux内核包含了几乎所有现代操作系统具有的同步机制，包括:原子操作，信号量，读写信号量，自旋锁，BKL,读写自旋锁，brlock(只在2.4内核中)，RCU(读拷贝更新/包含在2.6内核中)，顺序锁(包含在2.6内核中)

在linux中主要通过以下机制实现:
·semaphore(内核中的信号量)机制:
	linux内核的信号量概念和原理上与用户态的信号量是一样的，但是它不能再内核之外使用，它是一种睡眠锁.如果有一个任务想要获得已经被占用的信号量时，信号量会将这个进程放入一个等待队列，然后让其睡眠。当持有信号量的进程将信号量释放后，处于等待队列中的任务将被唤醒，并让其获得信号量.(见 xhl.jpg)
	信号量使用(定义在<asm/semaphore.h>中):
	1)定义信号量(struct semaphore sem;)
	2)初始化信号量如下：
	void sema_init(struct semaphore *sem,int val):该函数用于初始化信号量的初值，它设置信号量sem的值为val.
	void init_MUTEX(struct semaphore *sem):该函数用于初始化一个互斥锁，即把它信号量sem的值设置为1.
	void init_MUTEX_LOCKED(struct semaphore *sem):该函数也用于初始化一个互斥锁，但它把信号量sem的值设置为0，即一开始就处于锁态
	3)一步完成初始化的工作
	DECLARE_MUTEX(name):定义一个信号量name,并初始化它的值为1
	DECLARE_MUTEX_LOCKED(name):定义一个信号量name,但把它的值初始为0，即锁在创建时就处在已锁状态.
	4)获取信号量
	void down(struct semaphore *sem):获取信号量sem.可能会导致进程睡眠，因此不能在中断上下文使用该函数。该函数将把sem的值减1，如果信号量sem的值非负，就直接返回，否则调用者将被挂起，直到别的任务释放该信号量才能继续运行.
	void down_interruptible(struct semaphore *sem):获取信号量sem .如果信号量不可用，进程将被置为TASK_INTERRUPTIBLE类型的睡眠状态。该函数由返回值来区分是正常返回还是被信号中断返回，如果返回0，表示信号量正常返回，如果被信号打断，则返回-EINTR.
	down_killable(struct semaphore *sem):获取信号量sem.如果信号量不可用，进程将被置为TASK_KILLABLE类型的睡眠状态。
	注意：down()函数现已经不建议使用。建议使用down_interruptible()或down_killable()
	5)释放信号量
	void up(struct semaphore *sem):该函数释放信号量sem，即把sem的值加1，如果sem的值为非正数，表明有任务等待该信号量，因此唤醒这些等待者.

·spin_lock机制:
	自旋锁最多只能被一个可执行单元持有。自旋锁不会引起调用者睡眠，如果一个执行线程试图获得一个已经被持有的自旋锁，那么该线程就会一直进行忙循环，一直等待下去，在那里看是否该自旋锁的保持者已经释放了锁，"意思.
	自旋锁使用:
	1)spin_lock_init(x):该宏用于初始化自旋锁x,自旋锁在使用前必须先初始化.
	2)spin_lock(lock):获取自旋锁，如果成功，立即获得锁，并马上返回，否则它将一直自旋在那里，直到该自旋锁的保持者被释放.
	spin_trylock(lock):试图获取自旋锁lock,如果能立即获得锁，并返回真，否则立即返回假。它不会一直等待被释放.
	3)spin_unlock(lokc):释放自旋锁lock，它与spin_trylock或spin_lock配对使用.
	信号量PK自旋锁：见 xhlpkzxs.jpg
实例分析：5-6-2
-----------------------------------------------------------------------------
	实验要求5-6-1：
	1.在6410开发板上编写实现了读，
	写，定位功能的字符设备驱动程序
	2.编写应用程序，测试驱动
	参考答案：5-6-1

	实验要求5-6-2：
	基于实验6.1设计的驱动程序，加入竞争控制
	参考答案：5-6-2
	需修改的代码：
	1.memdev.c文件里面的mem_write方法修改
	2.memdev.c文件里面的memdev_init初始化信号量部分
	3.app-mem.c应用程序
