			一.mmap设备方法
描述：
内存映射函数mmap,负责把文件内容映射到进程的虚拟内存空间，通过对着段内存的读取和修改，来实现对文件的读写和修改，而不需要再调用read,write等操作.见 mmap.jpg

mmap系统调用:
	·void *mmap(void *addr,size_t len,int prot,int flags,int fd,off_t offset)
	1)addr:指定映射的起始地址，通常设为NULL,由系统指定。
	2)len:映射到内存的文件长度。
	3)prot:映射区的保护方式，可以是如下:
	PROT_EXEC:映射区可被执行
	PROT_READ:映射区可被读取
	PROT_WRITE:映射区可被写入
	4)flags:映射区的特性，可以是如下:
	MAP_SHARED:写入映射区的数据会复制回文件，且允许其他映射该文件的进程共享.
	MAP_PRIVATE:对映射区的写入操作会产生一个映射区的复制(copy-on-write),对比区域所做的修改不会写回原文件.
	5)fd:由open返回的文件描述符，代表要映射的文件.
	6)offset:以文件开始处的偏移量，必须是分页大小的整数倍，通常为0，表示从文件头开始映射.

解除映射:
	·int munmap(void *start,size_t length)
	1)start：取消参数所指向的映射内存
	2)length:表示欲取消的内存大小
	返回值:解除成功返回0，否则返回-1，错误原因存于errno中
	范例:mmap-app.c
	从上范例可发现mmap的特性，向文件里面写入数据时不会影响原有数据长度(原有数据多长就写多长，多余部分被截掉).

虚拟内存区域:
虚拟内存区域是进程的虚拟地址空间中的一个同质区间，即具有同样特性的连续地址范围。一个进程的内存映像由以下部分组成：程序代码，数据，BSS,栈区域，以及内存映射的区域.
	·一个进程的内存区域可以通过命令查看：vi /proc/<pid>/maps
	·每一行虚拟内存格式为：见 xnncgs.jpg
	·内核中虚拟内存结构(<linux/mm_types.h>)：vm_area_struct
	·主要成员:
	1)unsigned long vm_start:虚拟内存区域起始地址
	2)unsigned long vm_end:虚拟内存结束地址
	3)unsigned long vm_flags:
	VM_IO:将VMA标记为内存映射的IO区域，会阻止系统将该区域包含在进程的存放转存(core dump)中
	VM_RESERVED:标志内存区域不能够被换出

mmap设备操作:
映射一个设备是指把用户空间的一段地址关联到设备内存上。当程序读写这段用户空间的地址时，它实际上是在访问设备.
mmap也是file_oprations结构成员，mmap设备方法要做的就是建立虚拟地址到物理地址的页列表.
	·int (*mmap)(struct file *,struct vm_area_struct *)
	mmap完成页表的建立方法(如下有两种)
	1)使用remap_pfn_range一次建立所有页表(本次的)
	2)使用nopage VMA方法每次建立一个页表

页表的建立函数:
	·int remap_pfn_range(struct vm_area_struct *vma,unsigned long addr,unsigned long pfn,unsigned long size,pgprot_t prot)
	1)vma:虚拟内存区域指针
	2)addr: 虚拟地址的起始值
	3)pfn:要映射的物理地址所在的物理页号,可将物理地址>>PAGE_SHIFT得到
	4)size:要映射的区域大小
	5)prot:VMA的保护性
	范例:见 mmapfl.jpg
-----------------------------------------------------------------------------
			二.硬件访问
寄存器与内存：
寄存器和RAM的主要不同在于寄存器操作有副作用(side effect 或边际效果)读取某个地址时可能导致该地址内容发生变化，比如很多设备的中断状态寄存器只要一读取，便自动清零.
	·内存与I/O(空间):见 ncyio.jpg
		X86处理器中存在I/O空间的概念，I/O空间是相对内存空间而言的,它们是彼此独立的空间，32位X86系统I/O空间为64K，内存空间大小4G.
	·IO空间(有无):
		支持:X86
		不支持:arm mips powerPC
	·IO端口与IO内存：
		IO端口:当一个寄存器或内存位于IO空间时，称其为IO端口
		IO内存:当一个寄存器或内存位于内存空间时，称其为IO内存

操作IO端口(步骤):
	·申请
		struct resource *request_region(unsigned long first,unsigned long n,const chart *name)
		first:使用first开始的n个端口
		n:n个端口
		name:设备名字
		成功返回非NULL，申请失败返回NULL
		如果不能够分配需要端口可以查看/proc/ioports中的情况(是谁占用了)。
	·访问
		访问可分8,16,32位端口，<asm/io.h>定义了下列内联函数来访问IO端口
		unsigned inb(unsigned port):读8位
		void outb(unsigned char byte,unsigned port):写8位
		unsigned inw(unsigned port):读16位
		void outw(unsigned short byte,unsigned port):写16位
		unsigned inl(unsigned port):读32位
		void outl(unsigned longword byte,unsigned port):写32位
	·释放
	void release_region(unsigned long start,unsigned long n)

操作IO内存(步骤):
	·申请
		struct resource *request_mem_region(unsigned long start,unsigned long len,char *name)
		start:IO起始地址
		len:IO长度
		name:设备名字
		成功返回非NULL，申请失败返回NULL
		如果不能够分配需要端口可以查看/proc/iomem中的情况(是谁占用了)。
	·映射(物理地址到虚拟地址的映射)
		void *ioremap(unsigned long phys_addr,unsigned long size)
		phys_addr:物理地址
		size:大小
	·访问
		1)从IO内存读，使用下列之一
			1>unsigned ioread8(void *addr)
			2>unsigned ioread16(void *addr)
			3>unsigned ioread32(void *addr)
		2)从IO内存写，使用下列之一
			1>void iowrite8(u8 value,void *addr)
			2>void iowrite16(u16 value,void *addr)
			3>void iowrite32(u32 value,void *addr)
		老版本的IO内存读与写：见 lbbiodx.jpg
	·释放(步骤)
		1)void iounmap(void *addr)
		2)void release_mem_region(unsigned long start,unsigned long len)
-----------------------------------------------------------------------------
			三.混杂设备驱动
描述：
linux中存在一类字符设备，它们共享一个主设备号(10),但次设备号不同,我们称这类设备为混杂设备（miscdevice）.所有的混杂设备形成一个链表，对设备访问时内核根据次设备号查找到相应的miscdevice设备.

混杂设备结构：
	struct miscdevice{
	int minor;/*次设备号*/
	const char *name;/*设备名*/
	const struct file_operations *fops;/*文件操作*/
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	}

设备资源结构:
	struct resource{
	resource_size_t start;                  //资源的起始物理地址
	resource_size_t end;                    //资源的结束物理地址
	const char *name;                       //资源名称
	unsigned long flags;                    //资源的类型，如MEM,IO,IRQ
	struct resource *parent,*sibling,*child;//资源链表指针
	}

注册混杂设备：
	int misc_register(struct miscdevice *misc)

设备资源(范例):见 sbzyfl.jpg
-----------------------------------------------------------------------------
实验:
	5-8-1
	实验要求：
	1.基于实验6.1实现字符设备驱动程序，实现mmap设备方法.
	2.设计应用程序，测试驱动mmap
	参考答案:5-8-1
	需修改的代码：
	1.memdev.h结构，memdev.c里面mem_mmap方法实现
	2.memdev.c里面mem_vma_fault，vm_operations_struct

	5-8-2
	实验要求：
	1.修改LED驱动程序，实现控制LED全亮，全灭的命令
	2.设计应用程序，测试新添加的命令
	参考答案:5-8-2
	需修改的代码：
	1.led.c,led.h,app-led.c
	2.操作
	全亮:./app-led 1111
	全灭:./app-led 0000
