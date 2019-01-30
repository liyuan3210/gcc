			一.Linux进程控制
描述：
什么是程序，进程:
	·程序：程序是存放在磁盘上的一系列代码和数据的可执行映像，是一个静止的实体。
	·进程：进程是一个执行中的程序，它是动态的实体.
进程四要素：
	·有一段程序供其执行。这段程序不一定是某个进程所有，可以与其他进程共用。
	·有进程专用的内核空间堆栈。
	·在内核中有一个task_struct数据结构，即通常所说的"进程控制块"。有了这个数据结构，进程才能为内核调度的一个基本接受内核的调度。
	·有独立的用户空间,见：jcsys.jpg
进程描述：
	在linux中，线程，进程都使用struct task_struct来表示,它包含了大量描述进程/线程的信息，其中比较重要的信息有如下.
	·pid_t pid:进程号，最大值10亿
	·volatile long state:进程状态
		1)TASK_RUNNING
		进程正在被CPU执行，或者已经准备就绪，随时可以执行。当一个进程刚被创建时，就处于此状态。
		2)TASK_INTERRUPTIBLE
		处于等待中的进程，待等待条件为真实被唤醒，也可以被信号或者中断唤醒.
		3)TASK_UNINTERRUPTIBLE
		处于等待中的进程，待资源有效时唤醒，但是不可以由其它进程通过信号(signal)或中断唤醒.
		4)TASK_STOPPED
		进程中止执行。当接收到SIGSTOP和SIGTSTP等信号时，进程进入该状态，接收到SIGCONT信号后，进程重新回到TASK_RUNNING.
		5)TASK_KILLABLE
		linux2.6.25新引入的进程睡眠状态，原理类似于TASK_UNINTERRUPTIBLE,但是可以被致命信号(SIGKILL)唤醒.
		6)TASK_TRACED
		正处于被调试状态的进程.
		7)TASK_DEAD
		进程退出时(调用do_exit),state字段被设置为该状态.
	·int exit_state:进程退出时的状态
		1)EXIT_ZOMBIE(僵死进程)
		表示进程的执行被终止，但是父进程还没有发布waitpid()系统调用来收集有关死亡的进程信息.
		2)EXIT_DEAD(僵死撤销状态)
		表示进程的最终状态。父进程已经使用wait4()或waitpid系统调用来收集了信息,因此进程将由系统删除.
	·struct mm_struct *mm:进程用户空间描述指针，内核线程该指针为空。
	·unsigned int policy:该进程的调度策略。
	·int prio:优先级，相当于2.4中goodness()的计算结果，在0_(MAX_PRIO-1)之间取值(MAX_PRIO定义为140)，其中
		0_(MAX_RT_PRIO-1)(0_(MAX_RT_PRIO定义为100)属于实时进程范围，MAX_RT_PRIO-MX_PRIO-1属于非实时进程.（数值越大表示优先级越小）
	·static_prio：见 static_prio.jpg
	·struct sched_rt_entity rt:(rt->time_slice)
		时间片，进程缺省时间片与进程的静态优先级(在2.4中是nice值)相关，使用如下公式得出:
		MIN_TIMESLICE+((MAX_TIMESLICE-MIN_TIMESLICE)*(MAX_PRIO-1-(P)->static_prio)/(MAX_USER_PRIO-1))
		见 time_slice.jpg，task_struct2.4.jpg，task_struct2.6.jpg
	全局变量current:
		在linux中用current指针指向当前正在运行的进程task_struct.
		进程创建：见 create_procce.jpg
		进程撤销: 见 procce_eixt.jpg
-----------------------------------------------------------------------------
			二.Linux进程调度
描述:
	什么是进程调度?从就绪的进程中选出最适合的一个来执行,就叫进程调度.
	学习调度要掌握的方面(不管学linux还是wince的,都可以从这些方面进行学习):
	·调度策略
	·调度时机
	·调度步骤
调度策略:
	·SCHED_NORMAL(SCHED_OTHER):普通的分时进程
	·SCHED_FIFO:先入先出的实时进程
	·SCHED_RR:时间片轮转的实时进程
	·SCHED_BATCH:批处理进程
	·SCHED_IDLE:只在系统空闲时才能够被调度执行的进程
调度类:
	调度类的引入增强了内核调度程序的可扩展性，这些类(调度程序模块)封装了调度策略，并将调度策略模块化.
	·CFS调度类(在kernel/sched_fair.c中实现)用于以下调度策略:SCHED_NORMAL,SCHED_BATCH,SCHED_IDLE
	·实时调度类(在kernel/sched_rt.c中实现)用于以下调度策略:SCHED_RR，SCHED_FIFO
	调度类struct sched_class,里面pick_next_task(选择下一个运行的进程)：调度策略在此体现、

调度时机:
	`调度什么时候发生 ?即schedule()函数什么时候被调用？
调度发生有两种方式:
	·主动式
		在内核中直接调用schedule().当进程需要等待资源而暂停停止运行时，会把状态置于挂起（睡眠），并主动请求调度，让出cpu.
		例：
		current->state=TASK_INTERRUPTIBLE;
		schedule();
	·被动式(抢占)
		进程本身并不想放弃cpu，但是被其它进程给抢占cpu.
		用户抢占:linux2.4,linux2.6
发生在：
	·从系统调用返回用户空间
	·从中断处理程序返回用户空间
	内核即返回用户空间的时候，如果need_resched标志被设置，会导致schedule()被调用，此时就会发生用户抢占.
	见: yhqz文件夹
内核抢占:Linux2.6:
	·不支持内核抢占：
	进程/线程一旦运行内核空间，就可以一直执行，直到它主动放弃或时间片耗尽为止。这样一些紧急的进程或线程将长时间得不到运行。支持内核抢占(提高了系统实时性):更高优先级的进程/线程可以抢占正在内核空间运行的低优先级进程/线程。
	·以下情况不允许内核抢占如下(支持内核抢占的系统):
		1)内核正进行中断处理.进程调度函数schedule()会对此作出判断，如果是在中断中调用，会打印出错误信息.
		2)内核正在进行中断上下文的Bottom Half(中断的底半部)处理.硬件中断返回前会执行软中断，此时仍然处于中断上下文中.
		3)进程正持有spinlock自旋锁，writelock/readlock读写锁等，当持有这些锁时，不应该被抢占，否则由于抢占将导致其他cpu长期不能够获得而锁死等.
		4)内核正在执行调度程序scheduler.抢占的原因就是为了进行新的调度，没有理由将调度程序抢占掉再运行调度程序.
	·preempt_count:
		为了保证linux内核在以上情况下不会被抢占，使用了一个变量preempt_count,称为抢占计数。这一变量被设置在进程的thread_info结构中。每当内核要进入以上几种状态时，变量就加1，每当内核退出，就减1。同时进行可抢占的判断与调度.
		发生在:
		1)中断处理程序完成，返回内核空间之前.
		2)当内核代码再一次具有可抢占性的时候，如解锁及使能软中断等.
		见: nhqz

	·调度标志TIF_NEED_RESCHED:
		1)作用：
			内核提供了一个need_resched标志来表明是否需要重新执行一次调度
		2)设置：
			当某个进程耗尽它的时间片时，会设置这个标志；
			当一个优先级更高的进程进入可执行状态的时候，也会设置这个标志。
	·调度步骤:
		Schedule函数工作流程如下:
		1)清理 当前运行中的进程
		↓
		2)选出下一个要运行的进程(pick_next_task分析)
		↓
		3)设置新进程的运行环境
		↓
		4)进程上下文切换
-----------------------------------------------------------------------------
			三.Linux系统调用实现
描述：
	定义:一般情况下，用户进程是不能访问内核的，但linux内核中设置了一组用于实现各种系统功能的函数，称为系统调用.用户可以通过系统调用命令在自己的应用程序中调用它们.
	区别:系统调用与普通函数非常相似，但系统函数运行于内核态，普通函数运行于用户态.
	库函数:linux系统还提供了一些C语言函数库，这些库函数对系统调用进行了一些包装和扩展.
	不同版本内核所支持系统调用个数是不一样的.
系统调用原理:
	void main(){creat("testfile",0666)<0}
	1)应用程序首先用适当的值填充寄存器.
	·#define __NR_exit   (__NR_SYSCALL_BASE+  1)
	在<arch/arm/include/asm/unistd.h>
		↓
	2)然后调用特殊的指令跳到内核某一固定位置.
	·在intel中(由中断0x80实现),在arm中(由SWI[已经重命名SVC])
	·ENTRY(vector_swi)<arch/arm/kernel/entry-common.S>
		↓
	3)内核根据应用程序所填充的固定值来找到相应的函数执行.
	·从系统调用表sys_call_table找到相应的内核函数CALL(sys_exit)(arch/arm/kernel/calls.S).
	exit实现函数：exit.c(里面可能找不到sys_exit这个函数，因为它用SYSCALL_DEFINE3宏定义了)

实现系统调用步骤：
	·添加新的内核函数(arch/arm/kernel/sys.c)
		asmlinkage int sys_add(int a,int b)
		{
		return a+b;
		}
		asmlinkage//使用栈传递参数
	·更新头文件unistd.h
		#define _NR_add (_NR_SYSCALL_BASE+370)
	·针对这个新函数更新系统调用表calls.S
		CALL(sys_add)
	·编译内核。
	·编写应用程序调用
		main(){
		int result;
		result=syscall(370,1,2);
		printf("result=%d. \n",result);
		}
-----------------------------------------------------------------------------
			四.proc文件系统
描述：
	proc文件系统是内核与用户交互的一种机制,用户可以通过proc文件系统进行对内核的一个查看与配置.
	如查看内存使用情况:cat /proc/meminfo
	查询设备驱动号:cat /proc/devices
proc子目录:
	proc是系统根目录下面的文件,里面的子目录有如下:
	·apm:高级电源管理信息
	·bus:总线以及总线上的设备
	·devices:可用的设备信息
	·driver:已经启用的驱动程序
	·interrupts:中断信息
	·ioports:端口使用信息
	·version:内核版本
proc特点：
	·每个文件都规定了严格的权限(可读？可写？哪个用户可读？哪个用户可写?)
	·可以用文本编辑程序读取(more命令，cat命令，vi程序等)
	·不仅可以有文件，还可以有子目录
	·可以自己编写程序(内核程序)添加一个/proc目录下的文件
	·文件的内容都是动态创建的，并不存在于磁盘上(在内存上)
proc内核描述结构：
	struct proc_dir_entry{
	...
	read_proc_t *read_proc;
	write_proc_t *write_proc;
	...
	}
	·创建proc文件：
		struct proc_dir_entry *create_proc_entry(const char *name,mode_t mode,struct proc_dir_entry *parent)
		参数：
		name:要创建的文件名
		mode:要创建的文件属性默认0755
		parent:这个文件的父目录
	·创建proc目录：
		struct proc_dir_entry *proc_mkdir(const char *name,struct proc_dir_entry *parent)
		参数：
		name:要创建的目录名
		parent:这个目录的父目录
	·删除proc目录/文件：
		void remove_proc_entry(const char *name,struct proc_dir_entry *parent)
		参数：
		name:要删除的目录/文件
		parent:所在的父目录
	·读写(读写回调函数)
		read_proc：当用户使用vi及其它工具读取的时候就执行read_proc函数
		write_proc：当用户使用vi及其它工具写的时候就执行write_proc函数
	·读操作：
		int read_func(char *buffer,char *stat,off_t off,int count,int *peof,void *data)
		参数：
		buffer:把要返回给用户的信息写在buffer里，最大不超过PAGE_SIZE
		stat:一般不使用
		off:偏移量
		count:用户要取的字节数
		peof:读到文件尾时，需要把*peof置1
		data：一般不使用
	·写操作：
		int write_func(struct file *file,const char *buffer,unsigned long count,void *data)
		参数:
		file:该proc文件对应的file结构，一般忽略.
		buffer:待写的数据所在的位置
		count:待写数据的大小
		data：一般不使用

		实例分析(insmod proc.ko:运行后，cat /proc/proctest:读取会打印Helloword!)：sp1
		实例分析(echo 123 >/proc/mydir/pool:先写入，cat /proc/mydir/pool:然后再读出)：sp2
-----------------------------------------------------------------------------
			四.内核异常分析
oops分析(或者系统日志里面)：
	oops可以看成是内核级的segmentation fault.应用程序如果进行非法内存访问或执行，会得到segfault信号，一般的
	行为是coredump,应用程序也可以自己截获segfault信号，自行处理。如果内核自己犯了这个错误，则会打出oops信息。
oops分析步骤：
	·错误原因提示：见 cwts.jpg
	用来找到错误的信息及原因.
	·调用栈(对照反汇编代码): 见 oopsdz.jpg
	用来找到函数之间的调用顺序.
	·寄存器
	进行反汇编:arm-linux-objdump -D -S opps.ko >log
	根据cwts.jpg中的pc地址找到出错的地方

	实例分析:oops

 
