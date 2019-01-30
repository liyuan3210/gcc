			一.块设备简介
描述：
	块设备将数据存储在固定大小的块中，每个块的大小通常在512到32768字节之间。磁盘，SD卡都是常见的块设备.
块设备vs字符设备
	·块设备和字符设备最大的区别在于读写数据的基本单元不同。块设备读写数据的基本单元为块，例如磁盘通常为一个sector，而字符设备的基本单元为字节.
	·块设备能够随机访问，而字符设备则只能顺序访问.
体系架构:
	体系架构图:block_struct.jpg
	·虚拟文件系统VFS
		vm_vfs.jpg
	·Disk Cache
		当用户发起文件访问请求的时候，首先会到Disk cache中寻找文件是否被缓存了，如果再cache中，则直接从cache中读取，如果数据不在缓存中，就必须要到具体的文件系统中读取数据了.
	·Mapping Layer
		1).首先确定文件系统的block size,然后计算所请求的数据包含多少个block.
		2).调用具体文件系统的函数来访问文件的inode,确定所请求的数据在磁盘上的逻辑块地址.
	·Generic Block Layer
		linux 内核为块设备抽象了统一的模型，把块设备看作是由若干个扇区组成的数据空间。上层的读写请求在通用块层(Generic Block Layer)被构造成一个或多个bio结构。
	·I/O Scheduler Layer
		scheduler_layer.jpg
		电梯调度算法的基本原则：如果电梯现在朝上运动，如果当前楼层的上方和下方都有请求，则先响应所有上方的请求，然后才向下响应下方的请求；如果电梯向下运动，则刚好相反.
	·Block Device Driver
		device_driver.jpg
		
------------------------------------------------------------------------------------------
			二.块设备驱动程序设计
gendisk结构:
	在linux内核中使用strcut gendisk(定义于linux/genhd.h中)
	struct gendisk{
		int major;//主设备号
		int first_minor;//次设备号
		int minors;
		char disk_name[DISK_NAME_LEN];//驱动名
		struct block_device_operations *fops;
		struct request_queue *queue;//请求队列
		......
		int node_id;
	}
	·设备注册:
	void add_disk(struct gendisk *gd);

block_device_operations结构:
	字符设备通过file_operations结构来定义使它所支持的操作，块设备使用一个类似的结构block_device_operations.
	struct block_device_operations{
		int (*open)(struct block_device *,fmode_t);
		int (*release)(struct gendisk *,fmode_t);
		int (*ioctl)(struct block_device *,fmode_t,unsigned,unsigned log);
		......
	};
request结构:
	在linux中使用request表示等待处理的块设备IO请求.
	strcut request{
		struct list_head queuelist;//链表结构
		sector_t sector;//要操作的首个扇区
		unsigned long nr_sectors;//要操作的扇区数目

		struct bio *bio;//请求的bio结构体的链表
		struct bio *biotail;//请求的bio结构体的链表尾
		......
	};
	请求队列:
		简单的将，请求队列就是IO请求request所形成的队列，在linux内核中struct request_queue描述。
	·操作请求队列(linux提供了一系列函数用来操作请求队列):
	1)struct request_queue *blk_init_queue(request_fn_proc *rfn,spinlock_t *lock);
	初始化请求队列，一般在块设备驱动的模块加载函数中调用

	2)void blk_cleanup_queue(request_queue_t *q);
	清除请求队列，这个函数完成将请求队列返回给系统的任务，一般在块设备驱动模块卸载函数中调用.
	
	3）struct request *elv_next_request(request_queue_t *queue)
	返回下一个要处理的请求(由IO调度器决定)，如果没有请求则返回NULL.elv_next_request()不会清除请求，它任然将这个请求保留在队列上，因此连续调用它2次，2次会返回同一个请求结构体.

	4）void blkdev_dequeue_request(struct request *req);
	从队列中删除1个请求.

------------------------------------------------------------------------------------------
			三.SD卡测试


------------------------------------------------------------------------------------------
