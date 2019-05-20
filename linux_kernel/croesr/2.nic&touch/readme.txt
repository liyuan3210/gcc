			一.Linux网络子系统
linux的协议栈层次:
	·linux优点之一在于它丰富而稳定的网络协议栈.其范围从协议无关层(例如通用socket层接口或设备层)到各种具体的网络协议实现.
	协议栈层次对比:protocol_nic.jpg

linux网络子系统架构:
	linux网络子系统层次图:linux_nic.jpg

------------------------------------------------------------------------------------------
			二.网卡驱动程序设计
描述:
	每一个网络接口都由一个net_device结构来描述
net_device结构:
	·主要成员
		1)char name[IFNAMSIZ]:设备名，如：eth%d
		2)unsigned long state:设备状态
		3)unsigned long base_addr:I/O基地址
		4)unsigned int irq:中断号
		5)int (*init)(struct net_device *dev):初始化函数，该函数在register_netdev时被调用来完成对net_device结构的初始化

	·基本方法(必须)
		1)int (*open)(struct net_device *dev):打开接口,ifconfig激活时，接口将被打开。
			open请求任何它需要的系统资源并且启动接口：
			1>注册中断，DMA等
			2>设置寄存器，启动设备
			3>启动发送队列

			代码示例：
			int net_open(struct net_device *dev){
			/*申请中断*/
			request_irq(dev->irq,&net_interrupt,SA_SHIRQ,"dm9000",dev)

			/*设置寄存器，启动设备*/
			......

			/*启动发送队列*/
			netif_start_queue(dev);
			}

		2)int (*stop)(struct net_device *dev):停止接口。什么时候被调用呢？

		3)int (*hard_start_xmit)(struct sk_buff *skb,struct net_device *dev):数据发送函数
			当核心需要发送一个数据包时，它最终调用net_device结构中的hard_start_xmit函数指针。

	·可选方法(可选)
		1)int (*do_ioctl)(struct net_device *dev,strcut ifreq *ifr,int cmd):处理特定接口的ioctl命令
		2)int (*set_mac_address)(struct net_device *dev,void *addr):改变mac地址的函数，需要硬件支持该功能
	
	·动态分配
		1)struct net_device *alloc_netdev(int sizeof_priv,const char *mask,void (*setup)(struct net_device *));供协议栈代码使用
			sizeof_priv:私有数据区大小；mask:设备名；setup:初始化函数
		2)struct net_device *alloc_etherdev(int sizeof_priv);供协驱动代码使用

设备注册:
	网络接口驱动的注册方式与字符驱动不同之处在于它没有主次设备号，并使用如下函数注册
	·int register_netdev(strcut net_device *dev)

sk_buff结构:
	linux内核中的每个网络数据包都由一个套接字缓冲区结构struct sk_buff描述，即一个sk_buff结构就是一个包，指向sk_buff的指针通常被称作skb.

	·主要成员
		1)struct device *dev;//处理该包的设备
		2)__u32 saddr;//ip源地址
		3)__u32 daddr;//ip目的地址
		4)__u32 raddr;//ip路由器地址
		
		5)unsigned char *head;//分配空间的开始
		6)unsigned char *data;//有效数据的开始
		7)unsigned char *tail;//有效数据的结束
		8)unsigned char *end;//分配空间的结束
		9)unsigned char *len;//有效数据的长度
	
	·skb操作函数
		1)struct sk_buff *alloc_skb(unsigned int len,int priority):分配一个sk_buff结构，供协议栈代码使用
		2)struct sk_buff *dev_alloc_skb(unsigned int len):分配一个sk_buff结构，供协驱动代码使用

		3)unsigned char *skb_push(struct sk_buff *skb,int len):向后移动skb的tail指针，并返回tail移动之前的值。

		4)kfree_skb(struct sk_buff *skb):释放一个sk_buff结构，供协议栈代码使用
		5)dev_kfree_skb(struct sk_buff *skb):释放一个sk_buff结构，供协驱动代码使用

数据接收:
	网络接口驱动可以实现两种方式的报文接收，中断与查询。linux驱动多采用中断方式。接收流程如下:

	·分配skb
		skb=dev_alloc_skb(pkt->datalen+2)
	↓
	·从硬件中读取数据到skb
	↓
	·调用netif_rx将数据交给协议栈
		netif_rx(skb)

中断处理:
	网络接口通常支持3种类型的中断：新报文到达中断，报文发送完成中断，出错中断。中断处理程序可通过查看网卡中的中断状态寄存器，来分辨出中断类型。
------------------------------------------------------------------------------------------
			三.CS8900网卡驱动分析
CS8900描述:
	·CS8900是CIRRUS LOGIC公司生产的低功耗，性能优越的16位以太网控制器，功能强大。该芯片的突出特点是使用灵活，其物理层接口，数据传输模式和工作模式都能根据需要而动态调整，通过内部寄存器的设置来适应不同的应用环境.

	·CS8900内部功能模块主要是802.3介质访问控制块(MAC).802.3介质访问控制块支持全双工操作，完全依照IEEE802.3以太网标准，它负责处理有关以太网数据帧的发送和接收，包括：冲突检测，帧头的产生和检测，CRC效验码的生成和验证。通过对发送控制器(TxCMD)的初始化配置，MAC能自动完成帧的冲突后重传。如果帧的数据部分少于46个字节，它能生成填充字段使数据帧达到802.3所要求的最短长度.

	以太网帧:frame.jpg

寄存器:
	·LINECTL(0112H)
	LINECTL决定CS8900的基本配置和物理接口，例如：设置初始值为00d3H，选择物理接口为10BASE-T,并使能设备的发送和接收控制位.

	·RXCTL(0104H)
	RXCTL决定CS8900接收特定数据报，设置RXCTL的初始值为0d05H，表示接收网络上的广播或者目标地址同本地物理地址相同的正确数据包.

	·RXCFG(0102H)
	RXCFG决定CS8900接收到特定数据报后会引发接收中断，RXCFG设定为0103H，这样当收到一个正确的数据报后，CS8900会产生一个接收中断.

	·BUSCT(0116H)
	BUSCT可控制芯片的I/O接口的一些操作，设置初始值为8017H，打开CS8900的中断总控制位。

	·ISQ(0120H)
	ISQ是网卡芯片的中断状态寄存器，内部映射接收中断状态寄存器和发送中断状态寄存器的内容.

	·PORT0(000H)
	发送和接收数据时，CPU通过PORT0传递数据.

	·TXCMD(0004H)
	发送控制器，如果写入数据00c0H,那么网卡芯片在全都数据写入后开始发送数据.

	·TXLENG(0006H)
	发送数据长度寄存器，发送数据时，首先写入发送数据长度，然后将数据通过PORT0写入芯片.

	以上为几个最主要的工作寄存器(为16位),系统工作时，应首先对网卡芯片进行初始化，即写寄存器LINECTL,RXCTL,RCCFG,BUSCT

	发送数据时：
		1)写控制寄存器TXCMD,并将发送数据长度写入TXLENG
		↓
		2)然后将数据依次写入PORT0口，同网卡芯片将数据组织为链路层类型并添加填充位和CRC校验送到网络.

	接收数据时：
		1)分配skb
		skb=dev_alloc_skb(pkt->datalen+2);
		↓
		2)从硬件中读取数据到skb
		↓
		3)调用netif_rx将数据交给协议栈
		netif_rx(skb);


代码框架：
	·模块注册
	static int __init init_cs8900a_s3c2410(void){

		struct net_local *tp;
		int ret=0;
		
		dev_cs89x0.irq=irq;
		dev_cs89x0.base_addr=io;
		dev_cs89x0.init=cs89x0_probe.

		request_region(dev_cs89x0.base_addr,NETCARD_IO_EXTENT,"cs8900a");

		if(register_netdev(&dev_cs89x0)!=0);
	}

	·设备检测
	static int __init cs89x0_probe(struct net_device *dev,int ioaddr){

		/*get the chip type*/
		rev_type=readreg(dev,PRODUCT_ID_ADD);
		lp->chip_type=rev_type&-REVISION_BITS;
		lp->chip_revision=((rev_type&REVISON_BITS)>>8)+'A';

		if(lp->chip_type!=cs8900){
			printk(__FILE__"WRONG DEVICE DRIVER!\n");
			ret=-ENODEV;
			goto after kmalloc;
		}

		dev->dev_addr[0]=0x00;
		dev->dev_addr[1]=0x00;
		dev->dev_addr[2]=0xc0;
		dev->dev_addr[3]=0xff;
		dev->dev_addr[4]=0xee;
		dev->dev_addr[5]=0x08;
		set_mac_address(dev,dev->dev_addr);

		dev->irq=IRQ_LAN;
		printk(",IRQ%D",dev->irq);

		dev->open=net_open;
		dev->stop=net_close;
		dev->tx_timeout=net_timeout;
		dev->watchdog_time0=3*HZ;
		dev->hard_start_xmit=net_send_packet;
		dev->get_stats=net_get_stats;
		dev->set_multicast_list=set_multicast_list;
		dev->set_mac_address=set_mac_address;

		/*Fill in the fields of the device structure with ethernet values*/
		ether_setup(dev);
	}

	·数据发送
	staitc int net_send_packet(struct sk_buff *skb,struct net_device *dev){

		netif_stop_queue(dev);

		/*initiate a transmit sequence*/
		writeword(dev,TX_CMD_PORT,lp->send_cmd);
		writeword(dev,TX_LEN_PORT,skb->len);

		/*Test to see if the chip has allocated memory for the packet*/
		if((readreg(dev,PP_BusST)&READY_FOR_TX_NOW)==0){
		spin_unlock_irq(&lp->lock);
		DPRINTK(1,"cs89x0:Tx buffer not free!\n");
		return 1;
		}
		
		/*write the contens of the packet*/
		writeblock(dev,skb->data,skb->len);

		return 0;
	}
	
	·数据接收与中断
		1>中断函数
		static void net_interrupt(int irq,void *dev_id,struct pt_regs *regs){
			while((status==readword(dev,ISQ_PORT))){
				switch(status&ISQ_EVENT_MASK){
					case ISQ_RECEIVER_EVENT
					/*got a packet*/
					net_rx(dev);
					break;

					case ISQ_TRANSMITTER_EVENT
					lp->stats.tx_packets++;
					netif_wake_queue(dev);/*inform upper layers*/
					break;
				}
			}
		}

		2>接收函数
		static void net_rx(struct net_device *dev){
			status=inw(ioaddr+RX_RRAME_PORT);
			if((status&RX_OK)==0){
				count_rx_errors(status,lp);
				return;
			}

			length=inw(ioaddr+RX_FRAME_PORT);

			/*malloc up new buffer*/
			skb=dev_alloc_skb(length+2);

			if(skb==NULL){
				lp->stats.rx_dropped++;
				return;
			}
			skb_reserve(skb,2);

			/*mac头是14个字节，一开始保留两个，正是为了保证ip头的开始时四个字节对齐的*/
			skb->len=length;
			skb->dev=dev;
			readblock(dev,skb->data,skb->len);

			skb->protocol=eth_type_trans(skb,dev);
			netif_rx(skb);
		}
	
	部分代码:part_src.jpg

------------------------------------------------------------------------------------------
			三.input输入型驱动程序设计
描述：
	linux一种值得推荐的方法是利用input子系统来实现驱动。按键，触摸屏，鼠标等输入型设备都可以利用input接口函数来实现设备驱动.

体系结构：
	输入子系统由驱动层，核心层(input core),事件处理层(event handler)三部分组成。如一个鼠标移动，按键按下，通过driver->inputCore->event handler->userspace的顺序到达用户空间的应用程序.

	·输入子系统结构图：input_struct0.jpg input_struct1.jpg

	·各层次描述：
		1)驱动层
		将底层的硬件输入转化为统一事件模式，向输入核心(input core)汇报

		2)输入核心层
		为驱动层提供输入设备注册与操作接口，如:input_register_device;通知事件处理层对事件进行处理；在/proc下参数相应的设备信息.

		3)事件处理层
		主要作用是和用户空间交互，我们知道linux在用户空间将所有设备当成文件来处理，在一般的驱动程序中都有提供fops接口，以及在/dev下生成相应的设备文件nod,而在输入子系统中，这些工作都是由事件处理层完成的.

input_dev结构:
	在linux内核中，input设备用input_dev结构体来描述，使用input子系统实现输入设备驱动的时候，驱动的核心工作是向系统报告按键，触摸屏，键盘，鼠标等输入事件(event,通过input_event结构体描述)，不再需要关心文件操作接口，因为input子系统已经完成了文件操作接口。驱动报告的事件经过inputCore和Eventhandler最终到达用户空间.

	·注册输入设备
	int input_register_device(struct input_dev *dev);
	·注销输入设备
	void input_unregister_device(struct input_dev *dev);

驱动实现:
	·事件支持
	设备驱动通过set_bit()告诉input子系统它支持哪些事件，哪些键。例:
	set_bit(EV_KEY,button_dev.evbit);
		struct input_dev中有两个成员：
		1)evbit:事件类型
		2)keybit:按键类型
	事件类型:event_type.jpg

	·报告事件
	用于报告EV_KEY,EV_REL,EV_ABS事件的函数分别为:
		1)void input_report_key(struct input_dev *dev,unsigned int code,int value)
		2)void input_report_rel(struct input_dev *dev,unsigned int code,int value)
		3)void input_report_abs(struct input_dev *dev,unsigned int code,int value)
	参数：
		1)code
		事件的代码，如果事件类型是EV_KEY,该代码则为设备的键盘代码。例如键盘上的按键代码值为0至127，鼠标按键代码为0x110至0x116,其中ox110(BTN_LEFT)为鼠标左键，ox111(BTN_RIGHT)为鼠标右键，ox112(BTN_MIDDLE)为鼠标中键。其它代码含义请参考include/linux/input.h

		2)value
		事件的值，如果事件类型是EV_KEY,当按键按下时值为1，松开值为0.

	·报告结束
	input_sync()用于告诉input core，此次报告已结束。
	例(在触摸屏中，一次点击的整个报告过程如下)：
	input_report_abs(input_dev,ABS_X,x);//x坐标
	input_report_abs(input_dev,ABS_Y,y);//y坐标
	input_report_abs(input_dev,ABS_PRESSURE,1);
	input_sync(input_dev);//同步

实例分析:
	static void button_interrupt(int irq,void *dummy,struct pt_regs *fp){
		input_report_key(&button_dev,BTN_0,inb(BUTTON_PORT0));
		input_report_key(&button_dev,BTN_1,inb(BUTTON_PORT1));
		input_sync(&button_dev);
	}
	static int __init button_init(void){
		/*申请中断*/
		if(request_irq(BUTTON_IRQ,button_interrupt,0,"button",NULL))
		return -EBUSY;

		set_bit(EV_KEY,button_dev.evbit);//支持EV_KEY事件
		set_bit(BTN_0,button_dev.keybit);//设备支持的两个键
		set_bit(BTN_1,button_dev.keybit);
		input_register_device(&button_dev);//注册input设备

	}
------------------------------------------------------------------------------------------
			四.触摸屏驱动程序

------------------------------------------------------------------------------------------

