			一.PCI总线概述
描述：
	总线是一种传输信号的信道；总线是连接一个或多个导体的电器连接线；总线由如下组成：
	·电器接口
	·编程接口(我们关注的)

	PCI概念:
		·PCI是peripheral component interconnect(外围设备互联)的简称，是在桌面及其更大型的计算机上普片使用的外设总线.
		注意:在嵌入式中很难碰到PCI驱动开发，但是在嵌入式开发中会碰到很多类似PCI设备的变种.
	PCI优点:
		·在计算机和外设之间传输数据时具有更好的性能
		·能够尽量独立于具体的平台
		·可以方便地实现即插即用
PCI体系结构:
	·体系结构1
	1)从结构上看，PCI总线是一种不依赖附于某个具体处理器的局部总线，它是在CPU和原来的系统总线之间插入的一级总线，具体由一个桥接电路实现对这一层的管理，并实现上下之间的接口以协调数据的传递.
	结构图:pci_struct1.jpg
	2)系统的各个部分通过PCI总线和PCI-PCI桥连接在一起.CPU和RAM通过PCI桥连接到PCI总线0(即主PCI总线)，而具有PCI接口的显卡直接连接到主PCI总线上。
		1>PCI-PCI桥是一个特殊的PCI设备，它负责将PCI总线0和PCI总线1连接在一起。
		2>PCI1号总线上的是SCSI卡和以太网卡。
		3>ISA为了兼容旧的ISA总线标准，PCI总线还可以通过PCI-ISA桥来连接ISA总线，从而支持以前的ISA设备，图中ISA总线上连接着一个多功能I/O控制器，用于控制键盘，鼠标和软驱等.
	结构图:pci_struct2.jpg

PCI设备寻址:
	每个PCI设备由如下确定:
		·总线号(256)
		·设备号(32)
		·功能号(8)
	提示:"()"里面的数字表示PCI规范允许一个系统最多拥有的个数.
	
	/proc/iomem:
		描述了系统中所有的设备I/O在内存地址空间上的映射.我们来看地址从1G开始的第一个设备在:
	40000000-400003ff:0000:00:1f.1(这是一个PCI设备)
		·40000000-400003ff:映射的内存空间地址占据了内存地址空间1024bytes的位置。
		·0000:00:1f.1:则是这个PCI外设的地址，它以冒号和逗号分隔为4个部分
			1>0000：第一个16位表示域
			2>00  ：第二个8位表示一个总线号
			3>1f  ：第三个5位表示一个设备号
			4>1   ：第四个3位表示功能号
		由此可以得出上述的PCI设备的地址是0号总线上的31号设备上的1号功能。

查看系统PCI设备命令:lspci
		·通过本机lspci命令，画出本机pci设备结构.

配置寄存器:
	每个PCI设备都有一组固定格式的寄存器，即配置寄存器，配置寄存器由linux内核中的PCI初始化代码与驱动程序共同使用。内核在启动时负责对配置寄存器进行初始化，包括设置中断号以及I/O基地址等.

	寄存器结构图:register_strcut.jpg

	寄存器描述:
		·00H--01H Vendor ID 制造商标示
		·02H--03H Device ID 设备标识
		·04H--05H Command 命令寄存器
		·06H--07H Status  状态寄存器
		·08H revision id 版本识别号寄存器
		·09H--0bH Class code 分类代码寄存器
		·0cH Cache Line size CACHE行长度寄存器

		·0dH Latency Timer 主设备延迟时间寄存器
		·0eH Header Type 头标类型寄存器
		·0fH Bulit-in-teset register自测试寄存器
		·10H-13H Base Address register 0 基地址寄存器0
		·14H--17H Base Address register 1 基地址寄存器1
		·18H--1bH Base Address register 2 基地址寄存器2
		·1cH--19H Base Address register 3 基地址寄存器3
		
		·30H--33H Expasion Rom Base Address扩展ROM基地址
		·34H--3b  保留
		·3cH interrupt line 中断寄存器
		·3dH interrupt pin  中断引脚寄存器
		·3eH Min_Gnt 最小授权寄存器
		·3fH Max_Lat 最大延迟寄存器

------------------------------------------------------------------------------------------
			二.PCI驱动程序设计
pci_driver结构:
	·struct pci_driver{
		......
		/*id_table:这个驱动程序所支持的所有设备*/
		const struct pci_device_id *id_table;
		/*probe:当注册pci设备时，pci驱动程序会寻找系统中所支持的设备*/
		int (*probe)(struct pci_dev *dev,const struct pci_device_id *id);
		/*remove:拔掉时会调用*/
		void (*remove)(struct pci_dev *dev);
		/*device removed(NULL if not a hot-plug capable driver)*/
	}

	·注册PCI驱动
	pci_register_driver(struct pci_driver *drv);

	·使能设备
	在PCI驱动使用PCI设备的任何资源(I/O区域者中断)之前，驱动必须调用如下函数来使能设备:
	int pci_enable_device(struct pci_dev *dev);

	·获取基地址:
	一个pci设备最多可以实现6个地址区域。大多数pci设备在这些区域实现I/O寄存器。linux提供了一组函数来获取这些区间的基地址:
		1)pci_resource_start(struct pci_dev *dev,int bar)
		返回指定区域的起始地址，这个区域通过参数bar指定，范围从0-5，表示6个pci区域中的一个.
		2)pci_resource_end(struct pci_dev *dev,int bar)
		返回指定区域的末地址
中断:
	中断号存放于配置寄存器PCI_INTERRUPT_LINE中，驱动不必去检查它，因为从PCI_INTERRUPT_LINE中找到的值保证是正确的.如果设备不支持中断，寄存器PCI_INTERRUPT_LINE中的值是0，否则它是非0值。但因为驱动开发者通常知道设备是否支持终端，所以常常不需要访问PCI_INTERRUPT_LINE。

PCI网卡驱动程序:
	概述:
		该分析报告针对GNIC-II的千兆以太网卡，源程序文件：drivers/net/hamachi.c，由于该分析报告旨在对介绍PCI驱动程序结构，所以程序中关于硬件操作的具体部分不作介绍。
	·初始化
	static int __init hamachi_init(void){
		if(pci_register_driver(&hamachi_driver)>0)
			return 0;
		pci_unregister_driver(&hamachi_driver);
			return -ENODEV;
	}
	在模块初始化时采用pci_register_driver注册pci驱动程序。
	
	·hamachi_driver
	static struct pci_driver hamachi_driver={
	name:DRV_NAME;
	id_table:hamachi_pci_tbl;
	probe:hamachi_init_one;
	remove:__devexit_p(hamachi_remove_one);
	};

	·hamachi_pci_tbl
	static struct pci_device_id hamachi_pci_tbl[] __initdata={
	{0x1318,0x0911,PCI_ANY_ID,PCI_ANY_ID,},
	}
	该表记录的是该驱动程序能够支持的pci设备，分别是厂商号，设备号，子厂商号，子设备号，其中子厂商号，子设备号为PCI_ANY_ID，表示支持各种子类型。该表到底有什么实际用处?(通过分析函数pci_register_driver得出结论)
	
	·hamachi_init_one
	static int __init hamachi_init_one(struct pci_dev *pdev,const struct pci_device_id *ent){
		/*使能pci设备*/
		if(pci_enable_device(pdev)){
			ret=-EIO;
			goto err_out;
		}

		/*获取基地址*/
		ioaddr=pci_resource_start(pdev,0);

		/*申请将要使用的地址空间*/
		i=pci_request_regions(pdev,DRV_NAME);
		if(i)return i;

		/*获取中断号*/
		irq=pdev->irq;

		/*思考为什么这里要使用ioremap?*/
		ioaddr=(long)ioremap(loaddr,0x400);

		if(!ioaddr)
			goto err_out_release;

		dev=alloc_etherdev(sizeof(struct hamachi_private));
		if(!dev)
			goto err_out_iounmap;

		SET_MODULE_OWNER(dev);

		/*硬件相关操作，省略*/
			......

		/*The hamachi-specific entries in the device structure*/
		dev->open=&hamachi_open;
		dev->hard_start_xmit=&hamachi_start_xmit;
		dev->stop=&hamachi_close;
		dev->get_stats=&hamachi_get_stats;
		dev->set_multicast_list=&set_rx_mode;
		dev->do_ioctl=&netdev_ioctl;
		dev->tx_timeout=&hamachi_rx_timeout;
		dev->watchdog_timeo=TX_TIMEOUT;
		if(mtu)
			dev->mtu=mtu;

		i=register_netdev(dev);
		if(i){
			ret=i;
			goto err_out_unmap_rx;
		}
	}
	该函数的调用时机可通过分析pci_register_driver得出

------------------------------------------------------------------------------------------
			三.终端控制台体系
数据通信：
	数据通信的基本方式可分为并行通信与串行通信两种:
	·并行通信：
		利用多条数据线将数据的各位同时传送。它的特点是传输速度快，适用于短距离通信.
	·串行通信：
		利用一条数据线将数据一位位地顺序传送。特点是通信线路简单，利用简单的线缆就可实现通信，低成本，适用于远距离通信.

异步通信:
	·异步通信以一个字符为传输单位，通信中两个字符间的时间间隔是不固定的，然而同一个字符中的两个相邻之间的时间间隔是固定的.
	·通信协议：是指通信双方约定的一些规则，在使用异步串口传送一个字符的信息时，对数据格式有如下约定：规定有空闲位，起始位，资料位，奇偶校验位，停止位.
	·位描述:
		1)起始位：先发一个逻辑"0"信号，表示传输字符的开始
		2)数据位：紧接在起始位之后，数据位的个数可以使4,5,6,7,8等，从低位开始传送，靠时钟定位
		3)奇偶校验位：数据位加上这一位后，使得"1"的位数应为偶数(偶校验)或奇数(奇校验)，以此校验数据传送的正确性.
		4)停止位：它是一个字符数据的结束标志.
		5)空闲位：处于逻辑"1"状态，表示当前线路上没有数据传送.
		异步通信结构图:asynchronous.jpg

波特率：
	是衡量数据传送速率的指针。表示每秒钟传送的二进制位数。例如数据传送速率为120字符/秒，而每一个字符为10位，则其传送的波特率为10*120=1200位/秒=1200波特。
	注意：
	异步通信是按字符传输的，接收设备在收到起始信号之后只要在一个字符的传输时间内能和发送设备保持同步就能正确接收.

传送方式：
	transcation_method.jpg

linux终端概述:
	在linux中，TTY(终端)是一类字符设备的统称。
	包括了如下3种类型:
		1>控制台
		2>串口
		3>伪终端
	·控制台
	供内核使用的终端为控制台。控制台在linux启动时，通过命令console=...指定，如果没有指定控制台，系统把第一个注册的终端(tty)作为控制台.
		1>控制台是一个虚拟的终端，它必须映射到真正的终端上.
		2>控制台可以简单的理解为printk输出的地方.
		3>控制台是个只输出的设备，功能很简单，只能在内核中访问.
	·伪终端
	伪终端设备是一种特殊的终端设备，由主-从两个成对的设备构成，当打开主设备时，对应的从设备随之打开，形成连接状态.输入到设备的数据成为从设备的输出，输入到从设备的数据成为主设备的输出，形成双向管道.伪终端设备常用于远程登陆服务器来建立网络和终端的关联。当通过telnet远程登陆到另一台主机时，telnet进程与远程主机的telnet服务器相连接，telnet服务器使用某个主设备并通过对应的从设备与telnet进程相互通信.

终端体系：
	在linux中,TTY体系分为：
		1>TTY核心
		2>TTY线路规程
		3>TTY驱动3部分
	·TTY核心从用户获取要发送给TTY设备的数据，然后把数据传递给TTY线路规程，它对数据进行处理后，负责把数据传递到TTY驱动程序，TTY驱动程序负责格式化数据，并通过硬件发送出去.
	·从硬件收到的数据向上通过TTY驱动，进入TTY线路规程，再进入TTY核心，最后被用户获取。TTY驱动可以直接和TTY核心通讯，但是通常TTY线路规程会修改在两者之间传送的数据。TTY驱动不能直接和线路规程通信，甚至不知道它的存在，线路规程的工作时格式化从用户或者硬件收到的数据，这种格式化常常实现为一个协议，如PPP或Bluetooth.
	TTY体系图：
		1>tty_struct1.jpg
		2>tty_struct2.jpg

		TTY读操作：
		3>tty_read.jpg
		TTY驱动从硬件收到数据后，负责把数据传递到TTY核心，TTY核心将从TTY驱动收到的数据缓存到一个tty_flip_buffer类型的结构中。该结构包含两个数据组。从TTY设备接收到的数据被存储于第一个数组，当这个数组满，等待数据的用户将被通知。当用户从这个数组读取数据时，任何从TTY驱动新来的数据将被存储在第2个数组。当第二个数组存满后，数据再次提交给用户，并且驱动又开始填充第一个组，以此交替.
		
------------------------------------------------------------------------------------------
			四.串口驱动程序设计
uart_driver结构:
	struct uart_driver{
		struct module	*owner;
		const char	*driver_name;//驱动名
		const char	*dev_name;//设备名
		int		major	;//主设备号
		int		minor;//次设备号
		int		nr;//设备数
		struct console	*cons;
		struct uart_state *state;
		struct tty_driver *tty_driver;
	};
	·注册串口驱动
	int uart_register_driver(struct uart_driver *drv);

uart_port结构:
	uart_port用于描述一个uart端口(一个串口)的地址，FIFO大小，端口类型等信息。
	struct uart_port{
		spinlock_t lock;//端口锁
		unsigned int iobase;//IO端口基地址
		unsigned char __iomem *membase;//IO内存基地址
		unsigned int irq;//中断号
		unsigned char fifosize;//传输fifo大小

		const struct uart_ops *ops;
	}
	·uart_ops(关键是要实现里面的函数指针)
	uart_ops定义了针对串口的一系列操作，包括发送，接收及线路设置等.
	struct uart_ops{
		unsigned int(*tx_empty)(struct uart_port*);
		void(*set_mctrl)(struct uart_port *,unsigned int mctrl);
		unsigned int(*get_mctrl)(struct uart_port*);
		void(*stop_tx)(struct uart_port*);//停止发送
		void(*start_tx)(struct uart_port*);//开始发送
		void(*send_xchar)(struct uart_port*,char ch);//发送xchar
		void(*stop_rx)(struct uart_port*);//停止接收
		......
	}
	·添加端口
	int uart_add_one_port(struct uart_driver *drv,struct uart_port *port);

操作流程:
	·定义一个uart_driver的变量，并初始化;
	·使用uart_register_driver来注册这个驱动;
	·初始化uart_port和ops函数表;
	·调用uart_add_one_port()添加初始化好的uart_port.


------------------------------------------------------------------------------------------

