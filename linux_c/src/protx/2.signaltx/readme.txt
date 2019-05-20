			信号通信
描述：
   	信号通信是unix最为古老的进程间通信机制。

1.产生信号的条件
	1.当用户按某些键时。
	2.硬件异常信号，除数为0，无效的存储访问等..
	这些异常都由硬件检测到，将其通知内核。
	3.进程使用kill函数将信号发送给另一个进程。
	4.用户可以使用kill命令将信号发送给其它进程。

2.常见linux信号类型
	SIGHUP:从终端上发出的信号
	SIGINT:来至键盘的终端信号（ctrl+c）
	SIGTERM:kill命令发出的信号
	等等....

3.信号处理方式
	1.忽略此信号
	大多数信号都按照这种方式处理，其中有两种信号不能够被忽略
	SIGKILL，SIGSTOP，它们向超级用户提供了一种终止或停止的方法。
	2.执行用户希望的动作
	通知内核在某种信号发生时，调用一个用户函数。
	3.执行linux系统默认动作
	对大多数信号系统默认动作是终止该进程

4.信号发送
	1.int kill(pid_t pid,int signo)
	可以向自身和其它进程发送信号
	2.raise(int signo)
	只能够向自身发送信号
	3.unsigned int Alarm(unsigned int seconds)
	可以设置一个时间值，当时间到时时产生SIGALRM信号，如果捕捉不到，结束该进程
	4.int pause()
	使进程挂起，直至捕捉到一个信号为止

5.信号处理主要有
	1.使用简单的signal
	2.信号集函数组

6.signal使用(signal是个函数指针)：
	#include <signal.h>
	void(* signal(int signo,void(*func)(int)) )(int)
	func可能的值是：
	1.SIG_IGN
	2.SIG_DEL
	3.信号处理函数名
主要内容:
	1.运行一个main进程，在另外一个终端使用kill命令向mian进程发送信号

