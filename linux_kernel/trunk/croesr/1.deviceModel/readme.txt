			一.kobject&kset
sysfs文件系统:
	·sysfs是基于ram-based文件系统,ram-based文件系统有ramdisk，ramdisk是一种磁盘模拟技术，它之上运行的文件系统ex2.ex3.proc(基于内存文件系统)
		documentation/filesystems/sysfs.txt

	·作用：
		1)proc:让用户查看,修改linux内核系统信息参数。
		2)sysfs:内核数据结构，属性，关系展示给用户
		sysfs是从linux2.6引入的，与proc同及别文件系统.sysfs把连接在系统上的设备和总线组织成分级的文件，使其从用户空间可以访问到.

	·sysfs子目录
		1)block:在系统中发现的每个块块设备在该目录下对应一个子目录。每个字目录中又包含一些属性文件，它们描述了这个块设备的各个方面属性，如:块设备大小(loop块设备是使用此类文件来模拟的)
		2)bus:在内核中注册的每条总线在该目录下对应一个子目录。如：ide,pci,scsi,usb,pcmcia,其中每个子目录又包含两个子目录如下:
		devices:包含了在整个系统中发现的属于该总线类型的设备.
		drivers:包含了注册到该总线的所有驱动.
		3)class:将设备按照功能进行的分类，如/sys/class/net目录下包含了所有网络接口
		4)devices:包含系统所有的设备
		5)kernel:内核中的配置参数
		6)module:系统中所有模块的信息
		7)firmware:系统中的固件
		8)fs:描述系统中的文件系统
		9)power:系统中的电源选项
		sysfs框图:sysfs.jpg
kobject:
	·kobject实现了面向对象管理机制，是构成linux2.6设备模型的核心结构。它与sysfs文件系统紧密相连，在内核中注册的每个kobject对象对应sysfs文件系统的一个目录.

	·kobject结构:
	struct kobject{
		const char		*name;
		struct list_head	entry;
		struct kobject		*parent;//指向父对象
		struct kset		*kset;
		struct kobj_type	*ktype;
		struct sysfs_dirent	*sd;
		struct kref	kref;//对象引用计数
		unsigned int state_initialized:1;
		unsigned int state_in_sysfs:1;
		unsigned int state_add_uevent_sent:1;
		unsigned int state_remove_uenvent_sent:1;
	}
		1)struct kobj_type{
			void (*release)(struct kobject *kobj);
			struct sysfs_ops *sysfs_ops;
			struct attribute *default_attrs;
			}//记录kobject对象的一些属性
				1>release:用于释放kobject占用的资源，当kobject的引用计数为0时被调用.
				2>struct attribute{
				char * name;//属性文件名
				struct module * owner;
				mode_t mode;//属性的保护位
				}
				struct attribute(属性)：对应kobject的目录下的一个文件，name成员就是文件名
				3>struct sysfs_ops{
				ssize_t (*show)(struct kobject *kobj,struct attribute *attrib,char *);
				ssize_t (*store)(struct kobject *kobj,struct attribute *attrib,const char *,size_t);
				}
				show:当用户读属性文件时，该函数被调用，该函数将属性值存入buffer中并返回给用户态.
				store:当用户写属性文件时，该函数被调用，用于存储用户传入的属性值.

	·kobject操作
		1)void kobject_init(struct kobject *kobj);
		初始化kobject结构
		2)int kobject_add(struct kobject *kobj);
		将kobject注册到Linux系统
		3)int kobject_init_and_add(struct kobject *kobj,struct kobj_type *ktype,struct kobject * parent,const char *fmt,...);
		初始化并注册到linux系统
		4)void kobject_del(struct kobject *kobj);
		从linux系统中删除kobject对象
		5)struct kobject *kobject_get(struct kobject *kobj)
		将kobject对象的引用计数加1，同时返回该对象指针
		6)void kobject_put(struct kobject *kobj);
		将kobject对象的引用计数减1，如果引用计数降为0,则调用release方法释放该kobject对象
		

kset:
	·kset是具有相同类型的kobject的集合，在sysfs中体现成一个目录，在内核中用kset数据结构表示。
	kobject只能创建一个目录，而kset可以在目录里面再创建目录

	·kset结构
	struct kset{
		struct list_head list;//连接该kset中所有kobject的链表头
		spinlock_t list_lock;
		struct kobject kobj;内嵌的kobject
		struct kset_uevent_ops *uevent_ops;处理热插拔事件的操作集合
	}

		热插拔事件:
		在linux系统中，当系统配置发生变化时，如：添加kset到系统，移动kobject,一个通知会从内核空间发送到用户空间，这就是热插拔事件。热插拔事件会导致用户空间中相应的处理程序(如udev,mdev)被调用，这些处理程序会通过加载驱动程序，创建设备节点等来响应热插拔事件.
		1)struct kset_uevent_ops{
		int (*filter)(struct kset *kset,struct kobject *kobj);//决定是否将事件传递到用户空间，如果filter返回0将不传递事件.
		const char *(*name)(struct kset *kset,struct kobject *kobj);//用于将字符传递给用户空间的热插拔处理程序
		int (*uevent)(struct kset *kset,struct kobject *kobj,struct kobj_uevent_env *env);//将用户空间需要的参数添加到环境变量中
		}//当管理的文件发生变化时就调用上面三个函数
		kset原理:kset.jpg

	·kset操作
		1)int kset_register(struct kset *kset)
		在内核中注册一个kset
		2)void kset_unregister(struct kset * kset)
		从内核中注销一个kset

------------------------------------------------------------------------------------------
			二.总线设备驱动模型
描述:
	随着技术的不断进步，系统的拓扑结构也越来越复杂，对智能电源管理，热插拔的支持要求也越来越高，2.4内核已经难以满足这些需求。为适应这种形势的需求。linux2.6内核提供了全新的内核设备模型.

	·设备元素
	1)总线
	2)设备
	3)驱动

总线:
	总线是处理器和设备之间的通道，在设备模型中，所有的设备都通过总线相连，甚至是内部的虚拟"platform"总线。在linux设备模型中，总线由bus_type结构表示，定义在<linux/device.h>
	·bus_type结构
	struct bus_type{
		const char		*name;//总线名称
		struct bus_attribute	*bus_attrs;//总线属性
		struct device_attribute *dev_attrs;//设备属性
		struct driver_attribute *drv_attrs;//驱动属性
		int (*match)(struct device *dev,struct device_driver *drv);
		int (*uevent)(struct device *dev,struct kobj_uevent_env *env);
		int (*probe)(struct device *dev);
		int (*remove)(struct device *dev);
		void (*shutdown)(struct device *dev);
		int (*suspend)(struct *dev,pm_message_t state);
		int (*suspend_late)(struct device *dev,pm_message_t state);
		int (*resume_early)(struct device *dev);
		int (*resume)(struct device *dev);
		struct dev_pm_ops *pm;
		struct bus_type_private *p;
	}
		1)match
		当一个新设备或者驱动被添加到这个总线时该方法被调用。用于判断指定的驱动程序是否能处理指定的设备。若可以，则返回非零值.
		2)uevent
		在为用户空间产生热插拔事件之前，这个方法允许总线添加环境变量.
		3)bus_attribute属性
		struct bus_attribute{
		struct attribute attr;
		ssize_t (*show)(struct bus_type *bus_type,char *buf);
		ssize_t (*store)(struct bus_type *bus_type,const char *buf,size_t count);
		}
			1>创建属性:int bus_create_file(struct bus_type *bus,struct bus_attribute *attr)
			2>删除属性:void bus_remove_file(struct bus_type *bus,struct bus_attribute *attr)

	·bus_type操作
		1)bus_register(struct bus_type *bus)
		总线的注册,若成功，新的总线将被添加进系统，并可以再sysfs的/sys/bus下看到
		2)void bus_unregister(struct bus_type *bus)
		总线的删除

设备:
	·device结构
	struct device{
		......
		struct kobject		kobj;
		char bus_id[BUS_ID_SIZE];//在总线上唯一标示设备的字符串
		struct bus_type		*bus;//设备所在总线
		struct device_driver	*driver;//管理设备的驱动
		void *driver_data;//该设备驱动所使用的私有数据成员
		struct klist_node	knode_class;
		struct class		*class;
		struct attribute_group	**groups;
		void (*release)(strcut device *dev);
	}
		1)device_attribute属性
		struct device_attribute{
		struct attribute attr;
		ssize_t (*show)(struct device *dev,char *buf);
		ssize_t (*store)(struct  device *dev,struct device_attribute *attr,const char *buf,size_t count);
		}
			1>创建属性:int device_create_file(struct device *device,struct device_attribute *entry)
			2>删除属性:void device_remove_file(struct device *device,struct device_attribute *entry)

	·device操作
	1)int device_register(struct device *dev)
	注册设备
	2)void device_unregister(struct device *dev)
	注销设备

驱动:
	·device_driver
	struct device_driver{
		const char	*name;//驱动程序名字(体现在sysfs中)
		struct bus_type *bus;//驱动程序所在的总线
		struct module	*owner;
		const  char	*mod_name;
		int (*probe)(struct device *dev);//当驱动找到匹配的设备后就调用
		int (*remove)(struct device *dev);
		void (*shutdown)(struct device *dev);
		int (*suspend)(struct device *dev,pm_message_t state);
		int (*resume)(struct device *dev);
		struct attribute_group **groups;
		struct dev_pm_ops *pm;
		struct driver_private *p;
	}
		1)driver_attribute属性
		struct driver_attribute{
		struct attribute attr;
		ssize_t (*show)(struct device_driver *drv,char *buf);
		ssize_t (*store)(struct device_driver *drv,const char *buf,size_t count);
		}
			1>int driver_create_file(struct device_driver *drv,struct driver_attribute *attr);
			创建属性
			2>void driver_remove_file(struct device_driver *drv,struct driver_attribute *attr);
			删除属性
	·driver操作
		1)int driver_register(struct device_driver *drv);
		注册驱动
		2)void driver_unregister(struct device_driver *drv);
		注销驱动

------------------------------------------------------------------------------------------
			三.platform驱动程序设计
描述:
	platform总线是linux2.6内核加入的一种虚拟总线。platform机制的本身使用并不复杂，由两部分组成:platform_device和platform_driver
	·platform优势:platform驱动与传统的设备驱动模型相比，优势在于platform机制将设备本身的资源注册进内核，由内核统一管理，在驱动程序使用这些资源时使用统一的接口，这样提高了程序的可移植.
	platform流程:platform.jpg

platform_device:
	·platform_device结构
	struct platform_device{
		const char *name;//设备名
		int id;//设备编号，配合设备名使用
		struct device dev;
		u32 num_resources;
		struct resource *resource;//设备资源
	}
		1)resource设备资源
			struct resource{
			resource_size_t start;//资源的起始物理地址
			resource_size_t end;//资源的结束物理地址
			const char *name;//资源的名称
			unsigned long flags;//资源的类型，比如mem,io,irq类型
			struct resource *parent,*sibling,*child;//资源链表指针
		}
			1>设备资源例子:device_dom.jpg
			2>获取资源:struct resource *platform_get_resource(struct platform_device *dev,unsigned int type,unsigned int num);
			dev:资源所属的设备
			type:获取资源类型
			num:获取资源数
			例(获取中断号)：platform_get_resource(pdev,IORESOURCE_IRQ,0)

		

	·platform操作
	1)struct platform_device *platform_device_alloc(const char *name,int id);
	分配platform_device结构
	name:设备名称、
	id:设备id，一般为-1
	2)int  platform_device_add(struct  platform_device *pdev);
	注册平台设备

platform_driver:
	·platform_driver结构
	struct platform_driver{
		int (*probe)(struct platform_driver *);
		int (*remove)(struct platform_driver *);
		void (*shutdown)(struct platform_driver*);
		int (*suspend)(struct platform_driver *,pm_message_t state);
		int (*suspend_late)(struct platform_driver *,pm_message_t state);
		int (*resume_early)(struct platform_driver*);
		int (*resume)(struct platform_driver *);
		struct platform_driver driver;
	}
	
	·platform_driver操作
	1)int platform_driver_register(struct platform_driver*);
	平台驱动注册

-----------------------------------------------------------------------------------------
			四.中断处理
描述：
	·linux中断实现步骤:
		1)向内核注册中断
		2)实现中断处理函数

向内核注册中断：
	·int request_irq(unsigned int irq,void (*handler)(int,void*,struct pt_regs *)，unsigned long flags,const char *devname,void *dev_id);
		返回0表示成功，否则返回一个错误码
		1)irq:中断号
		2)handler:中断处理函数
		3)flags:与中断处理相关的各种选项
			1>IRQF_DISABLED(SA_INTERRUPT)
				1.如果设置该为，表示是一个"快速"中断处理程序(当串口中断里面嵌有并口中断时，并口中断将被忽略)；
				2.如果没有设置这位，那么是一个"慢速"中断处理程序(允许中断嵌套)；
			2>IRQF_SHARED(SA_SHIRQ)
				1.表明中断可以再设备间共享,不同的设备可以挂到同一个中断信号线上
				特别之处:
				1)dev_id参数必须是唯一的
				2)disable_irq(unsigned int irq)不能使用(如果使用了共享中断线上的其它设备同样无法使用了)
		4)devname:设备名称
		5)dev_id:共享中断时使用

实现中断处理函数:
	·中断处理程序就是普通的c代码，特别之处在于c程序是在"中断上下文"中运行的，与之对应的是"进程上下文".
		它受到了某些限制（如下）:
		1)不能向用户空间发送或接受数据
		2)不能使用可能引起阻塞的函数
		3)不能使用可能引起调度的函数

	·中断处理函数流程
		void short_sh_interrupt(int irq,void *dev_id,strcut pt_regs *regs){
		/*判断是否是本设备产生了中断(为什么要做判断)*/
		value=inb(short_base);
		if(!(value&0x80))return;

		/*清除中断位(如果设备支持自动清除，则不需要这步)*/
		outb(value&ox7F,short_base);

		/*中断处理，通常是数据接收*/
		......

		/*唤醒等待的进程*/
		wake_up_interruptible(&short_queue);
		}

	·操作
		1)void free_irq(unsigned int irq,void *dev_id);
		当设备不再需要中断时(通常在驱动卸载时),应当把他们返还给系统.

	

------------------------------------------------------------------------------------------
			五.按键程序设计

------------------------------------------------------------------------------------------

