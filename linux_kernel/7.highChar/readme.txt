			一.Ioctl设备控制 
描述：
	大部分驱动除了需要具备读写设备能力外，还需要具备对硬件控制的能力。如需要设备报告错误信息，改变波特率，这些操作常常通过Ioctl方法来实现。

用户空间Ioctl方法：
	在用户空间，使用Ioctl系统调用来控制设备，原型如下
	int ioctl(int fd,unsigned long cmd,...)
	原型中的点表示这是一个可选的参数，存在与否依赖于控制命令(第2个参数)是否涉及到与设备的数据交互。

驱动Ioctl方法：
	·Ioctl驱动方法与内核版本不同的原型.
	1)int (*ioctl)(struct inode *inode,struct file * filp,unsigned int cmd,unsigned long arg)(2.6.36已废弃)
	2)long (*unlocked_ioctl)(struct file *filp,unsigned int cmd,unsigned long arg)(2.6.36核以后的)
	cmd参数从用户空间传下来，可选的参数arg以一个unsigned log的形式传递，不管它是一个整数或一个指针。如果cmd命令不涉及数据传输，则第3个参数arg的值无任何意义.

	·定义命令
	为了防止对错误的设备使用正确的命令，命令号应该在系统范围内是唯一的。ioctl命令编码被划分为几个段，include/asm/ioctl.h中定义了这些位字段类型如下，在linux-2.6.39\Documentation\ioctl\ioctl-number.txt内核文件中罗列了已经使用了的幻数.
	类型(幻数)/Type：表明哪个设备的命令，参考ioctl-number.txt之后选出，8位宽.
	序号/number：序号,表明设备命令中的第几个，8位宽.
	传送方向/direction：数据传送的方向，可能的值是_IOC_NONE(没有数据传输)，_IOC_READ,_IOC_WRITE.数据传送石从应用程序的观点看待的，_IOC_READ意思是从设备读.
	参数大小/size：用户数据的大小(13/14位宽，视处理器而定).
	宏定义(内核提供)：
	1)_IO(type,nr):没有参数的命令.
	2)_IOR(type,nr,datatype):从驱动中读取数据.
	3)_IOW(type,nr,datatype):写数据到驱动.
	4)_IOWR(type,nr,datatype):双向传送，type和number成员作为参数被传递.
	定义命令/范例：
	#define MEM_IOC_MAGIC 'm'//定义幻数
	#define MEM_IOCSET
	_IOW(MEM_IOC_MAGIC,0,int)
	#define MEM_IOCGQSET
	_IOR(MEM_IOC_MAGIC,1,int)

	·实现命令
	unlocked_ioctl函数实现包括如下三个技术环节,
	1)返回值
	unlocked_ioctl函数实现通常是根据命令执行的一个switch语句。当命令号不匹配任何一个设备所支持的命令时，通常返回-EINVAL(非法参数).
	2)参数使用
	如果是一个整数，可以直接使用。如果是指针，我们必须确保这个用户地址是有效的，因此使用前需要进行正确的检查
	不需要检测：copy_from_user,copy_to_user,get_user,put_user
	需要检测：__get_user,__put_user
	使用int access_ok(int type,const void *addr,unsigned long size)来进行检测
	type：VERIFY_READ或VERIFY_WRITE用来表明是读用户内存还是写用户内存.
	addr:是要操作的用户内存地址.
	size:是操作的长度.
	如果Ioctl需要从用户空间读取一个整数，那么size参数等于sizof(int).
	access_ok返回一个布尔值：1(表示成功)，0(表示失败)如果函数返回失败，则loctl应当返回-EFAULT.

	·实现命令/范例:
	if(_IOC_DIR(cmd)&_IOC_READ)
	   err=!access_ok(VERIFY_WRITE,(void __user *)arg,_IOC_SIZE(cmd));
	//why _IOC_READ对应VERIFY_WRITE?
	else if(_IOC_DIR(cmd)&_IOC_WRITE)
	   err=!access_ok(VERIFY_READ,(void __user *)arg,_IOC_SIZE(cmd));//相对用户空间要验证VERIFY_READ
	if(err)
	   return -EINVAL;

	·命令操作
	switch(cmd){
	case MEM_IOCSQUANTUM:
	...
	break;
	case MEM_IOCGQUANTUM:
	...
	break;
	default:
	return -EINVAL;
	}
-----------------------------------------------------------------------------
			二.内核等待队列/阻塞型字符设备驱动
描述：
	在linux驱动程序设计中，可以使用等待队列来实现进程的阻塞，等待队列可看作保存进程的容器，在阻塞进程时，将进程放入等待队列，当唤醒进程时从等待队列中取出进程.

操作等待队列：
	·linux2.6内核提供如下相关等待队列操作
	1)定义等待队列:wait_queue_head_t my_queue
	2)初始化等待队列:init_waitqueue_head(&my_queue)
	3)定义并初始化等待队列(上面两步合并一步完成):DECLARE_WAIT_QUEUE_HEAD(my_queue)

	·有条件睡眠
	1)wait_event(queue,condition)
	当condition(布尔表达式)为真时，立即返回；否则让进程进入TASK_UNINTERRUPTIBLE模式的睡眠，并挂在queue参数所指定的等待队列上.
	2)wait_event_interruptible(queue,condition)
	当condition(布尔表达式)为真时，立即返回；否则让进程进入TASK_INTERRUPTIBLE模式的睡眠，并挂在queue参数所指定的等待队列上.
	3)int wait_event_killable(wait_queue_t queue,condition)
	当condition(布尔表达式)为真时，立即返回；否则让进程进入TASK_KILLABLE模式的睡眠，并挂在queue参数所指定的等待队列上.

	·无条件睡眠(老版本，建议不再使用)
	1)sleep_on(wait_queue_head_t *q)
	让进程进入不可中断的睡眠，并把它放入等待队列q
	2)interruptible_sleep_on(wait_queue_head_t *q)
	让进程进入可中断的睡眠，并把它放入等待队列q.

	·从等待队列中唤醒进程
	1)wake_up(wait_queue_t *q)
	从等待队列q中唤醒状态为TASK_UNINTERRUPTIBLE,TASK_INTERRUPTIBLE,TASK_KILLABLE的所有进程。
	2)wake_up_interruptible(wait_queue_t *q)
	从等待队列q中唤醒状态为TASK_INTERRUPTIBLE的进程。

阻塞型字符设备驱动:
	当一个设备无法立刻满足用户的读写请求时应该如何处理?驱动程序应当(缺省地)阻塞进程，使它进入睡眠，直到请求可以得到满足。
	阻塞方式：是文件读写操作的默认方式.
	非阻塞方式：但应用程序可通过O_NONBLOCK标志来人为的设置读写操作为非阻塞方式(该标志定义在<linux/fcntl.h>中，在打开文件时指定),如果进程在没有数据就绪时调用了read,或者在缓冲区没有空间时调用了write，系统只是简单的返回-EAGAIN,而不会阻塞进程.

-----------------------------------------------------------------------------
			三.poll设备方法
描述：见 poll.jpg

select系统调用：
	select系统调用用于多路监控，当没有一个文件满足要求时，select将阻塞调用进程.
	·int select(int maxfd,fd_set *readfds,fd_set *writefds,fe_set *exceptfds,const struct timeval *timeout)
	maxfd:文件描述的范围，比待检测的最大文件描述文件符大1
	readfds:被读监控的文件描述符集
	writefds:被写监控的文件描述符集
	exceptfds:被异常监控的文件描述符集
	timeout:定时器
	1)timeout为0，不管是否有文件满足要求，都立刻返回，无文件满足要求返回0，有文件满足要求返回一个正值
	2)timeout为NULL,select将阻塞进程，直到某个文件满足要求
	3)timeout为正整数，就是等待的最长时间，即select在timeout时间内阻塞进程

	·select系统调用返回值:
	1)正常情况下返回满足要求的文件描述符个数
	2)经过了timeout等待后仍无文件满足要求，返回值0
	3)如果select被某个信号中断，它将返回-1并设置errno为EINTR
	4)如果出错，返回-1并设置相应的errno

select系统调用(使用方法):
	·将要监控的文件添加到文件描述符集
	↓
	·调用select开始监控
	↓
	·判断文件是否发生变化

系统宏对描述符的操作：
	#include <sys/select.h>
	void FD_SET(int fd,fd_set *fdset):宏FD_SET将文件描述符fd添加到文件描述符集fdset中;
	void FD_CLR(int fd,fd_set *fdset):宏FD_CLR将文件描述符集fdset中清除文件描述符fd;
	void FD_ZERO(fd_set *fdset):清空文件描述符集fdset;
	void FD_ISSET(int fd,fd_set *fdset):在调用select后使用FD_ISSET来检测文件描述符集fdset中的文件fd发生了变化;
范例：
	FD_ZERO(&fds);//清空集合
	FD_SET(fd1,&fds);//设置描述符
	FD_SET(fd2,&fds);//设置描述符
	maxfdp=fd1+1;//描述符最大值加1，假设fd1>fd2
	switch(){
	case -1:exit(-1);break;//select错误，退出程序
	case 0:break;
	default:
	   if(FD_ISSET(fd1,&fds))//测试fd1是否可读
	}

poll方法实现：
	应用程序常常使用select系统调用，它可能会阻塞进程。这个调用由驱动的poll方法实现.
	unsigned int(*poll)(struct file *filp,poll_table *wait)
	·poll负责完成：
	1)使用poll_wait将等待队列添加到poll_table中.
	↓
	2)返回描述设备是否可读或可写的掩码.
		位掩码如下
		1>POLLIN:设备可读
		2>POLLRDNORM:数据可读
		3>POLLOUT:设备可写
		4>POLLWRNORM:数据可写
		设备可读通常返回(POLLIN|POLLRDNORM)
		设备可写通常返回(POLLOUT|POLLWRNORM)

	·范例：
	staitc unsigned int mem_poll(struct file*filp,poll_table*wait){
	struct scull_pipe *dev=filp->private_data;
	unsigned int mask=0;
	//把进程添加到等待队列
	poll_wait(filp,&dev->inq,wait);
	//返回掩码
	if(有数据可读)
	   mask=POLLIN|POLLRDNORM;//设备可读
	return mask;
	}
工作原理：poll方法只是做一个登记，真正的阻塞发生在select.c中的do_select函数。(可分析内核代码)
-----------------------------------------------------------------------------
			四.自动创建设备文件
描述：
设备文件创建有两种一种是用mknod手动创建，还一种是现在要讲的自动创建.
2.4内核创建方法：
	devfs_register(devfs_handle_t dir,const char *name,unsigned int flags,unsigned int major,unsigned int minor,umode_t mode,void *ops,void *info)
	dir:在指定的目录中创建设备文件,dir为目录，为空表示在/dev目录下创建.
	name:文件名
	flags:创建标志
	major:主设备号
	minor:次设备号
	mode:创建模式
	ops:操作函数集
	info:通常为空
2.6.13内核创建:
	从linux2.6.13开始，devfs不复存在，udev成为devfs的替代。相比devfs,udev(mdev)存在于应用层.利用udev(mdev)来实现设备文件的自动创建很简单，在驱动初始化的代码里调用class_create为该设备创建一个class,再为每个设备调用device_create创建对应的设备.
	范例:
	struct class *myclass=class_create(THIS_MODULE,"my_device_driver");
	device_create(myclass,NULL,MKDEV(major_num,0),NULL,"my_device");
	当驱动被加载时，udev(mdev)就会自动在/dev下创建my_device设备文件.
-----------------------------------------------------------------------------
实验:
	实验-7.1
	实验要求：
	1.基于实验6.1设计的字符设备驱
	动，加入Ioctl的支持
	2.设计应用程序，测试Ioctl功能
	参考答案：5-7-1
	需修改的代码：
	1.memdev.c文件里面的memdev_ioctl方法,memdev_init初始化函数
	2.memdev.h头文件里面定义的幻数与命令
	3.app-ioctl.c应用程序

	实验-7.2
	实验要求：
	1.对实验6.1实现的字符设备驱动程
	序进行读，写改造.
	读:该驱动在设备无数据可读时发生阻塞.
	写:该驱动在设备没空间可写时发生阻塞.
	2.设计应用程序，测试阻塞型读写
	参考答案： 5-7-2
	需修改的代码：
	1.memdev.c文件里面的mem_read,mem_write方法修改,memdev_init初始化函数
	2.memdev.h头文件里面定义
	3.app-block.c应用程序

	实验-7.3
	实验要求：
	1.基于实验7.2实现的驱动程序，添
	加poll设备方法。
	2.设计应用程序，测试驱动程序的
	poll操作。
	参考答案： 5-7-3
	需修改的代码：
	1.memdev.c文件里面的mem_poll方法修改
	2.app-poll.c应用程序

	实验-7.4
	实验要求：
	在实验7.3的基础上，实现设备文件
	的自动创建。
	参考答案：5-7-3
	需修改的代码：
	1.添加create_devices,delete_devices方法.
	2.查看/dev/目录下面是否有memdev0,memdev1两个文件，或者重启开发板执行程序测试
