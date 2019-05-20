#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#define PERM S_IRUSR|S_IWUSR
			/*共享内存*/
//创建共享内存，在父进程中把参数传到共享内存中在子进程中读取。
void main(int argc,char * argv[]){
	int shmid;
	char *p_addr,*c_addr;
	//判断参数
	if(argc==1){
		printf("please add a argv!\n");
		exit(1);
	}
	//创建共享内存
	if((shmid=shmget(IPC_PRIVATE,1024,PERM))==-1){
		printf("creat share memory error!\n");
		exit(1);
	}
	//创建子进程
	if(fork()){
		p_addr=shmat(shmid,0,0);//映射，第二个参数为0表示让系统自己获取内存地址
		memset(p_addr,'\n',1024);//清存
		strncpy(p_addr,argv[1],1024);
		shmdt(p_addr);//脱离共享内存
		wait(NULL);
		exit(0);
	}else{//子进程读
		sleep(1);//防止先进入子进程
		c_addr=shmat(shmid,0,0);//映射
		printf("child process get %s \n!",c_addr);
		shmdt(c_addr);//脱离共享内存
		exit(0);
	}

}
