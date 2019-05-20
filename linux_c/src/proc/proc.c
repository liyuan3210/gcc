#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
			/*进程控制*/
/*获取线程id*/
void getproid(){
	printf("pid=%d\n",getpid());//获取本进程id
}
/*pid_t fork()创建进程*/
//在父进程中：fork()返回子进程的PID(大于0)
//在子进程中：fork()返回0
//出现错误：fork()返回一个负值
void creatpro(){
	pid_t pid;
	int count=0;
	//创建一个进程(此时只有一个进程)
	pid=fork();
	count++;
	printf("count=%d\n",count);//count会打印两次每次都为1(因为每创建一个进程都会copy自己的数据)
	//此时已经有两个进程
	if(pid<0){
	printf("creat pro failure!\n");
	}else if(pid==0){
	printf("into child %d id!\n",getpid());//获取本进程id
	}else{
	printf("into parent %d id!\n",getpid());//获取本进程id
	}
}
/*vfork创建进程*/
//区别：
//1.vfork会共享数据,而fork会copy两个独立的数据
//1.vfork子进程先运行 负进程后运行，而fork执行顺序不确定
///////////////////////////////还没测试成功/////////////////////////////////////////
void creatvpro(){
	pid_t pid;
	int count=0;
	//创建一个进程(此时只有一个进程)
	pid=vfork();
	count++;
	printf("count=%d\n",count);//count会打印两次每次都为1(因为每创建一个进程都会copy自己的数据)	
}
/*exec函数*/
//区别
//exec与fork,vfork相比，exec不会新创建一个新的进程与Pid，pid保持不变并启动一个新的程序，替换原有的进程，
void execFunction(){
	int execid=0;
	/*int execl(path,commName,arg1,arg2,...),成功不返回值，失败返回-1，原因在errno中*/
	//path(完整路径)，commName(命令)，中间是需要的参数列表，(char*)0（表示没有参数需要放了）
	execid=execl("/home/ly/apl/proc/execMain","./execMain","liyuan","litianyuan",(char*)0);

		/*int execlp(path,commName,arg1,arg2,...)*/
	//与execl不同的是path将从linux环境变量path中去找
	execid=execlp("execMain","execMain","liyuan","litianyuan",(char*)0);
	
	/*int execv(path,char * argv[])*/
	//与execl不同的是它把参数放到argv数组里面了(path是完整路径)
	char * argv[]={"./execMain","liyuan","litianyuan",(char*)0};
	execid=execv("/home/ly/apl/proc/execMain",argv);
	
	/*system*/
	//system会调用fork产生一个子进程，在子进程中执行
	system("./execMain liyuan litianyuan");
	
	/*wait阻塞该进程,直到某个子进程退出*/
	wait(NULL);
	
	
}
void main(){
	//getproid();
	//creatpro();
	//creatvpro();
	//execFunction();
	
}

