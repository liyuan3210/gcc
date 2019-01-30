#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>
			/*信号通信*/
/*运行一个main进程，在另外一个终端使用kill命令向mian进程发送信号*/
//测试方法
//使用kill命令来进行测试，查看进程id命令 ps au 或 ps aux
//发送信号命令kill -s SIGINT 6069
void myfunc(int sign_no){
	if(sign_no==SIGINT){
	printf("I have get SIGINT! \n");
	}else if(sign_no==SIGQUIT){
	printf("I have get SIGQUIT! \n");
	}else if(sign_no==1){
	printf("ok! \n");
	}
}
void main(){
	printf("waiting for signal SIGINT or SIGQUIT!\n");
	//注册信号
	signal(SIGINT,myfunc);
	signal(SIGQUIT,myfunc);
	//使进程挂起，直至捕捉到一个信号为止
	pause();
	exit(0);
}
